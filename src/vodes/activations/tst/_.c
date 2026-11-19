// tst: block_activate, a right click on a block uses it by its kind (block_activate_system), in an all air chunk
//  (air_chunk.c, no game, no player)
//  - a door placed with ground under it; a clicker in front of its top (TriggerActionB, RaycastRange, a terrain link,
//    no camera: chunk3_raycast_system casts from its own position along its facing)
//  - out of reach (its range under the distance) a click does nothing; in reach it opens the door (both parts'
//    block_data_open); turned away onto a bed behind it: it sleeps there (Sleeping, SleepBlock the bed's base), the
//    door left open; woken and put back, facing the door again it closes
//  - run: bash bsh/tst.sh block_activate
byte zox_tst_bact_stage = 0;
int zox_tst_bact_frames = 0;
int3 zox_tst_bact_base, zox_tst_bact_bed;
float3 zox_tst_bact_at;
entity zox_tst_bact_clicker = 0;

static inline byte zox_tst_bact_open(ecs* world, entity terrain, int y) {
    const int3 p = { zox_tst_bact_base.x, zox_tst_bact_base.y + y, zox_tst_bact_base.z };
    return (terrain_block_data_at(world, terrain, p) & block_data_open) != 0;
}

// a turn about up whose facing (as the raycast reads it) is along z, +1 or -1
static inline float4 zox_tst_bact_facing(float z) {
    for (int i = 0; i < 4; i++) {
        const float4 q = quaternion_from_axis_angle(float3_up, i * (float) M_PI * 0.5f);
        if (quaternion_to_normal(q).z * z > 0.9f) {
            return q;
        }
    }
    return quaternion_identity;
}

static inline void zox_tst_bact_click(ecs* world, float range, float facing) {
    zox_setv(zox_tst_bact_clicker, RaycastRange, range);
    zox_setv(zox_tst_bact_clicker, Rotation3D, zox_tst_bact_facing(facing));
    zox_setv(zox_tst_bact_clicker, TriggerActionB, zox_dirty_trigger);
    zox_tst_bact_frames = 0;
}

byte zox_tst_block_activate_ready(ecs* world) {
    const entity air = zox_tst_streamed_air_chunk(world);
    if (!air) {
        return 0;
    }
    const entity terrain = zox_tst_world_terrain;
    const entity realm = zox_tst_world_realm;
    const float block = zox_getv(terrain, BlockScale);
    zox_tst_bact_frames++;
    zox_tst_progress(zox_tst_bact_stage * 1000 + zox_tst_bact_frames);
    if (zox_tst_bact_stage == 0) {
        const int length = octree_size(zox_getv(terrain, NodeDepth));
        const int3 chunk = zox_getv(air, ChunkPosition);
        zox_tst_bact_base = (int3) { chunk.x * length + 6, chunk.y * length + 4, chunk.z * length + 8 };
        const byte door = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockDoor)), BlockIndex);
        zox_tst_check(zox_tst_place_supported(world, terrain, realm, zox_tst_bact_base, door, 0), "the door wasn't placed");
        // a bed behind the clicker, at the door's top's height, its near face 1.5 blocks away
        zox_tst_bact_bed = (int3) { zox_tst_bact_base.x, zox_tst_bact_base.y + 1, zox_tst_bact_base.z - 4 };
        const byte bed = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockBed)), BlockIndex);
        zox_tst_check(zox_tst_place_supported(world, terrain, realm, zox_tst_bact_bed, bed, 0), "the bed wasn't placed");
        // two blocks in front (-z) of its top's centre, at that height
        const float3 top = float3_scale(int3_to_float3((int3) { zox_tst_bact_base.x, zox_tst_bact_base.y + 1, zox_tst_bact_base.z }), block);
        zox_tst_bact_clicker = zox_new();
        zox_tst_bact_at = float3_add(top, (float3) { 0.5f * block, 0.5f * block, -1.5f * block });
        zox_set(zox_tst_bact_clicker, Position3D, { zox_tst_bact_at });
        zox_set(zox_tst_bact_clicker, Rotation3D, { zox_tst_bact_facing(1) });
        zox_set(zox_tst_bact_clicker, TriggerActionB, { 0 });
        zox_set(zox_tst_bact_clicker, RaycastRange, { 4 * block });
        zox_set(zox_tst_bact_clicker, RaycastVoxelData, { 0 });
        zox_set_link(world, zox_tst_bact_clicker, TerrainLink, terrain);
        zox_tst_bact_stage = 1;
        zox_tst_bact_frames = 0;
        return 0;
    }
    // a few frames for the queue to place it, its ray to cast, and each click (trigger, then active)
    if (zox_tst_bact_frames < 5) {
        return 0;
    }
    const RaycastVoxelData* ray = zox_get(zox_tst_bact_clicker, RaycastVoxelData);
    if (zox_tst_bact_stage == 1) {
        zox_tst_waiting = "the door placed and in the clicker's ray";
        if (!terrain_voxel_at(world, terrain, zox_tst_bact_base) || !ray->result) {
            return zox_tst_bact_frames > 300;
        }
        zox_log("[tst] the ray hits [%i] at [%.2f] blocks", ray->result, ray->distance / block);
        // out of reach: its range under the door's distance (the ray still finds it, the raycast caps at its range)
        zox_tst_bact_click(world, ray->distance * 0.9f, 1);
        zox_tst_bact_stage = 2;
        return 0;
    }
    if (zox_tst_bact_stage == 2) {
        zox_tst_check(!zox_tst_bact_open(world, terrain, 0), "a click out of reach opened the door");
        zox_tst_bact_click(world, 4 * block, 1);
        zox_tst_bact_stage = 3;
        return 0;
    }
    if (zox_tst_bact_stage == 3) {
        zox_tst_check(zox_tst_bact_open(world, terrain, 0) && zox_tst_bact_open(world, terrain, 1),
            "a click in reach on the door's top didn't open it, base [%i] top [%i]", zox_tst_bact_open(world, terrain, 0),
            zox_tst_bact_open(world, terrain, 1));
        zox_tst_bact_click(world, 4 * block, -1);
        zox_tst_bact_stage = 4;
        return 0;
    }
    if (zox_tst_bact_stage == 4) {
        zox_tst_check(zox_tst_bact_open(world, terrain, 0), "a click on the bed closed the door");
        zox_tst_check(zox_has(zox_tst_bact_clicker, Sleeping), "a click on the bed didn't put it to sleep");
        const int3 sleep = zox_has(zox_tst_bact_clicker, SleepBlock) ? zox_getv(zox_tst_bact_clicker, SleepBlock) : int3_zero;
        zox_tst_check(int3_equals(sleep, zox_tst_bact_bed), "it sleeps at [%i %i %i], not the bed's base [%i %i %i]", sleep.x, sleep.y, sleep.z,
            zox_tst_bact_bed.x, zox_tst_bact_bed.y, zox_tst_bact_bed.z);
        character_wake(world, zox_tst_bact_clicker);
        zox_tst_bact_stage = 5;
        zox_tst_bact_frames = 0;
        return 0;
    }
    if (zox_tst_bact_stage == 5) {
        // off the bed (its climb)
        if (zox_has(zox_tst_bact_clicker, SleepMotion)) {
            return zox_tst_bact_frames > 600;
        }
        zox_tst_check(!zox_has(zox_tst_bact_clicker, Sleeping), "woken, it's still asleep");
        zox_setv(zox_tst_bact_clicker, Position3D, zox_tst_bact_at);
        zox_tst_bact_click(world, 4 * block, 1);
        zox_tst_bact_stage = 6;
        return 0;
    }
    zox_tst_check(!zox_tst_bact_open(world, terrain, 0) && !zox_tst_bact_open(world, terrain, 1),
        "a click on the door didn't close it, base [%i] top [%i]", zox_tst_bact_open(world, terrain, 0), zox_tst_bact_open(world, terrain, 1));
    zox_delete(zox_tst_bact_clicker);
    zox_tst_bact_stage = 7;
    return 1;
}

void zox_tst_block_activate_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_bact_stage == 7, "it stopped at stage [%i]", zox_tst_bact_stage);
}

#include "door_rays.c"

void zox_tsts_activations(ecs* world) {
    zox_tst_add("block_activate", zox_tst_block_activate_ready, zox_tst_block_activate_check, 60);
    zox_tst_set_setup("block_activate", NULL, 1);
    zox_tsts_door_rays(world);
}
