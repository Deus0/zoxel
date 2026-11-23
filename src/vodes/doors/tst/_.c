#include "door_light.c"
#include "door_sky.c"
#include "door_save.c"
#include "door_glow.c"
// tst: doors, a door opens and closes (vodes/doors), in a game near its player (block voxes spawn only while it plays)
//  - placed (turned a quarter) with ground under it, its block vox spawns closed: its parts solid to collisions
//  - toggled through its top (a filler): both parts' block data open, collisions pass through both
//  - its sound plays (DoorSound handled); its block vox swings a quarter turn about its hinge: the hinge stays, the door's middle ends half a block to its back
//  - its block datas saved and loaded keep the open state; its block vox respawned starts open, no swing
//  - toggled again through its base: closed, solid, swung back to rest (a right click: tst block_activate)
//  - run: bash bsh/tst.sh doors
#define zox_tst_doors_direction 1
byte zox_tst_doors_stage = 0;
int zox_tst_doors_frames = 0;
double zox_tst_doors_time = 0;
int3 zox_tst_doors_base;
entity zox_tst_doors_vode = 0;

// solid to terrain collisions (the same check physics makes, intersect2.c)
static inline byte zox_tst_doors_solid(ecs* world, entity terrain, entity realm, int3 block) {
    const collision_terrain t = { world, zox_get(realm, BlockLinks), zox_get(terrain, ChunkLinks), zox_getv(terrain, NodeDepth),
        zox_getv(terrain, BlockScale) };
    collision_chunk_cache cache = { 0 };
    return collision_block_solid(&t, &cache, block);
}

static inline int3 zox_tst_doors_top() {
    return (int3) { zox_tst_doors_base.x, zox_tst_doors_base.y + 1, zox_tst_doors_base.z };
}

// both parts open (or closed) in block data and to collisions
static inline void zox_tst_doors_check_parts(ecs* world, entity terrain, entity realm, byte open) {
    const int3 parts[2] = { zox_tst_doors_base, zox_tst_doors_top() };
    for (byte i = 0; i < 2; i++) {
        const byte on = terrain_active_at(world, terrain, parts[i]);
        zox_tst_check((on != 0) == open, "the door's %s is on [%i], not %s", i ? "top" : "base", on, open ? "open" : "closed");
        zox_tst_check(zox_tst_doors_solid(world, terrain, realm, parts[i]) == !open, "the door's %s is %s to collisions",
            i ? "top" : "base", open ? "solid" : "passable");
    }
    zox_tst_check(block_data_direction(terrain_block_data_at(world, terrain, zox_tst_doors_base)) == zox_tst_doors_direction,
        "the door lost its direction");
}

// its hinge stays, and its middle (half a block along its x from the hinge) points along its own x closed, its back open
static inline void zox_tst_doors_check_swing(ecs* world, entity vode, float block, byte open) {
    const float3 rest = zox_getv(vode, DoorRestPosition);
    const float4 rest_rotation = zox_getv(vode, DoorRestRotation);
    const float3 hinge = zox_getv(vode, DoorHinge);
    const float3 position = zox_getv(vode, Position3D);
    const float4 rotation = zox_getv(vode, Rotation3D);
    const float3 hinge_rest = float3_add(rest, float4_rotate_float3(rest_rotation, hinge));
    const float3 hinge_now = float3_add(position, float4_rotate_float3(rotation, hinge));
    zox_tst_check(float3_distance(hinge_rest, hinge_now) < 0.01f * block, "the hinge moved [%.3f]", float3_distance(hinge_rest, hinge_now));
    const float3 middle = float3_sub(float3_add(position, float4_rotate_float3(rotation, float3_add(hinge, (float3) { 0.5f * block, 0, 0 }))), hinge_now);
    const float3 expected = float4_rotate_float3(rest_rotation, open ? (float3) { 0, 0, 0.5f * block } : (float3) { 0.5f * block, 0, 0 });
    zox_tst_check(float3_distance(middle, expected) < 0.01f * block, "the door's middle is [%.2f %.2f %.2f] from its hinge, not [%.2f %.2f %.2f] (%s)",
        middle.x, middle.y, middle.z, expected.x, expected.y, expected.z, open ? "open" : "closed");
    zox_tst_check(zox_getv(vode, DoorSwing) == (open ? 1 : 0), "its swing is [%.2f]", zox_getv(vode, DoorSwing));
}

byte zox_tst_doors_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const float block = zox_getv(terrain, BlockScale);
    zox_tst_doors_frames++;
    zox_tst_progress(zox_tst_doors_stage * 1000 + zox_tst_doors_frames);
    if (zox_tst_doors_stage == 0) {
        zox_tst_waiting = "air for a door";
        // air a block round it, up to its top and one over, in the player's chunk column
        const int length = octree_size(zox_getv(terrain, NodeDepth));
        const int3 chunk = real_position_to_chunk_position(zox_getv(character, Position3D), length, block);
        for (int y = 0; y < length * 2 - 4; y++) {
            const int3 base = { chunk.x * length + 6, chunk.y * length + y, chunk.z * length + 8 };
            int solid = 0;
            for (int x = -1; x <= 1; x++) for (int h = 0; h < 3; h++) for (int z = -1; z <= 1; z++) {
                solid += terrain_voxel_at(world, terrain, (int3) { base.x + x, base.y + h, base.z + z }) != 0;
            }
            if (solid) {
                continue;
            }
            const byte door = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockDoor)), BlockIndex);
            zox_tst_check(zox_tst_place_supported(world, terrain, realm, base, door, zox_tst_doors_direction), "the door wasn't placed");
            zox_tst_doors_base = base;
            zox_tst_doors_stage = 1;
            zox_tst_doors_frames = 0;
            return 0;
        }
        return zox_tst_doors_frames > 600;
    }
    if (zox_tst_doors_stage == 1) {
        zox_tst_waiting = "the door's block vox";
        zox_tst_doors_vode = terrain_vode_at(world, terrain, zox_tst_doors_base);
        if (!zox_valid(zox_tst_doors_vode) || !zox_has(zox_tst_doors_vode, DoorOpen)) {
            if (zox_tst_doors_frames % 200 == 0) {
                zox_log("[tst] base voxel [%i] vode [%lu] door [%i]", terrain_voxel_at(world, terrain, zox_tst_doors_base), zox_tst_doors_vode,
                    zox_valid(zox_tst_doors_vode) && zox_has(zox_tst_doors_vode, DoorOpen));
            }
            return zox_tst_doors_frames > 1200;
        }
        zox_tst_doors_check_parts(world, terrain, realm, 0);
        zox_tst_doors_check_swing(world, zox_tst_doors_vode, block, 0);
        zox_tst_check(door_toggle(world, terrain, realm, zox_tst_doors_top()), "toggling the door through its top did nothing");
        zox_tst_doors_check_parts(world, terrain, realm, 1);
        zox_tst_doors_stage = 2;
        zox_tst_doors_time = zox_current_time;
        return 0;
    }
    if (zox_tst_doors_stage == 2) {
        zox_tst_waiting = "the door swinging open";
        if (zox_getv(zox_tst_doors_vode, DoorSwing) < 1) {
            return zox_current_time - zox_tst_doors_time > 10;
        }
        zox_log("[tst] opened in [%.2fs]", zox_current_time - zox_tst_doors_time);
        zox_tst_check(zox_current_time - zox_tst_doors_time >= door_swing_time * 0.9f, "it opened in [%.2fs], under its swing time",
            zox_current_time - zox_tst_doors_time);
        zox_tst_doors_check_swing(world, zox_tst_doors_vode, block, 1);
        zox_tst_check(!zox_has(zox_tst_doors_vode, DoorSound), "its sound wasn't played (DoorSound still on it)");
        // its chunk's sections through a file: the open state kept, its block datas too
        byte3 local;
        const entity chunk = terrain_chunk_local(world, terrain, zox_tst_doors_base, &local);
        FILE* file = tmpfile();
        zox_tst_check(file && save_chunk_sections(file, zox_get(chunk, BlockDatas), zox_get(chunk, ActiveOctree)),
            "its block datas and ActiveOctree didn't save");
        if (file) {
            rewind(file);
            BlockDatas loaded = { 0 };
            ActiveOctree loaded_actives = { 0 };
            zox_tst_check(load_chunk_sections(file, &loaded, &loaded_actives), "its sections didn't load");
            zox_tst_check(getv_ActiveOctree(&loaded_actives, zox_getv(chunk, NodeDepth), local), "the open state wasn't saved");
            zox_tst_check(loaded.length == zox_get(chunk, BlockDatas)->length, "loaded [%i] bytes of block data, saved [%i]",
                loaded.length, zox_get(chunk, BlockDatas)->length);
            resize_BlockDatas(&loaded, 0);
            collapse_ActiveOctree(&loaded_actives);
            fclose(file);
        }
        // its block vox spawned again (as a chunk loading does): open from the start
        spinlock* lock = zox_octree_lock(world, chunk);
        zox_octree_lock_begin(lock);
        destroy_node_link_VoxelNode(world, getm_VoxelNode(zox_getm(chunk, VoxelNode), zox_getv(chunk, NodeDepth), local));
        zox_octree_lock_end(lock);
        zox_add(chunk, VoxelNodeDirty);
        zox_tst_doors_stage = 3;
        zox_tst_doors_frames = 0;
        return 0;
    }
    if (zox_tst_doors_stage == 3) {
        zox_tst_waiting = "the door's block vox spawned again";
        const entity vode = terrain_vode_at(world, terrain, zox_tst_doors_base);
        if (!zox_valid(vode) || vode == zox_tst_doors_vode || !zox_has(vode, DoorOpen)) {
            return zox_tst_doors_frames > 1200;
        }
        zox_tst_doors_vode = vode;
        zox_tst_doors_check_swing(world, vode, block, 1);
        zox_tst_check(door_toggle(world, terrain, realm, zox_tst_doors_base), "toggling the door through its base did nothing");
        zox_tst_doors_check_parts(world, terrain, realm, 0);
        zox_tst_doors_stage = 4;
        zox_tst_doors_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the door swinging closed";
    if (zox_getv(zox_tst_doors_vode, DoorSwing) > 0) {
        return zox_current_time - zox_tst_doors_time > 10;
    }
    zox_tst_doors_check_swing(world, zox_tst_doors_vode, block, 0);
    const float3 position = zox_getv(zox_tst_doors_vode, Position3D), rest = zox_getv(zox_tst_doors_vode, DoorRestPosition);
    zox_tst_check(float3_distance(position, rest) < 0.001f, "closed it isn't at rest, [%.3f] off", float3_distance(position, rest));
    zox_tst_doors_stage = 5;
    return 1;
}

void zox_tst_doors_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_doors_stage == 5, "it stopped at stage [%i]", zox_tst_doors_stage);
}

void zox_tsts_doors(ecs* world) {
    zox_tst_add("doors", zox_tst_doors_ready, zox_tst_doors_check, 120);
    zox_tst_add("door_light", zox_tst_door_light_ready, zox_tst_door_light_check, 120);
    zox_tst_add("door_save", zox_tst_door_save_ready, zox_tst_door_save_check, 120);
    zox_tst_set_setup("door_save", NULL, 1);
    zox_tst_add("door_glow", zox_tst_door_glow_ready, zox_tst_door_glow_check, 120);
    zox_tst_set_setup("door_glow", NULL, 1);
    zox_tst_add("door_sky", zox_tst_door_sky_ready, zox_tst_door_sky_check, 120);
}
