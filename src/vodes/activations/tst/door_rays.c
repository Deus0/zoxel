// tst: door_rays, rays at a door find that door, not its neighbour or nothing (block_activate_system's ray)
//  - two doors side by side (the second a block along x), in a game near its player (block voxes spawn only then)
//  - rays at the first's face from in front (-z) and behind (+z), across it (a tenth, the middle, nine tenths) at
//    its base's and its top's height: each finds the first door's base (multi_block_base_at), closed; open, the same
//    rays moved onto its panel along its block's side
//  - facing its first way (direction 0, a block vox with no turn) it swings open: it had no Rotation3D, its swing
//    system never ran, it stayed shut
//  - a play test: placing a second door, clicks on the first toggled the second, or nothing
//  - run: bash bsh/tst.sh door_rays
#define zox_tst_dr_rays 12
byte zox_tst_dr_stage = 0;
int zox_tst_dr_frames = 0;
int3 zox_tst_dr_base;
entity zox_tst_dr_clickers[zox_tst_dr_rays];
int zox_tst_dr_found[2], zox_tst_dr_other[2], zox_tst_dr_missed[2];

static inline void zox_tst_dr_measure(ecs* world, entity terrain, entity realm, byte open) {
    const float block = zox_getv(terrain, BlockScale);
    for (int i = 0; i < zox_tst_dr_rays; i++) {
        const RaycastVoxelData* ray = zox_get(zox_tst_dr_clickers[i], RaycastVoxelData);
        const float3 at = zox_getv(zox_tst_dr_clickers[i], Position3D);
        int3 base = int3_zero;
        entity kind = 0;
        if ((ray->result == rayhit_terrain || ray->result == rayhit_block_vox) && zox_valid(ray->chunk) &&
            zox_has(ray->chunk, ChunkPosition))
        {
            kind = multi_block_base_at(world, terrain, realm, chunk_block_position(world, ray->chunk, ray->positionl), &base);
        }
        const byte found = kind && zox_has(kind, BlockDoor) && int3_equals(base, zox_tst_dr_base);
        if (found) {
            zox_tst_dr_found[open]++;
        } else if (kind) {
            zox_tst_dr_other[open]++;
        } else {
            zox_tst_dr_missed[open]++;
        }
        if (!found) {
            zox_log("[tst] door_rays %s: the ray from [%.2f %.2f %.2f] (blocks off the base) hit [%i] base [%i %i %i], not the door at [%i %i %i]",
                open ? "open" : "closed", at.x / block - zox_tst_dr_base.x, at.y / block - zox_tst_dr_base.y,
                at.z / block - zox_tst_dr_base.z, ray->result, base.x, base.y, base.z, zox_tst_dr_base.x, zox_tst_dr_base.y, zox_tst_dr_base.z);
        }
    }
}

byte zox_tst_door_rays_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const float block = zox_getv(terrain, BlockScale);
    zox_tst_dr_frames++;
    zox_tst_progress(zox_tst_dr_stage * 1000 + zox_tst_dr_frames);
    if (zox_tst_dr_stage == 0) {
        zox_tst_waiting = "air for two doors";
        // air round them, up to their tops and one over, in the player's chunk column (as tst doors)
        const int length = octree_size(zox_getv(terrain, NodeDepth));
        const int3 chunk = real_position_to_chunk_position(zox_getv(character, Position3D), length, block);
        byte placed = 0;
        for (int y = 0; y < length * 2 - 4 && !placed; y++) {
            const int3 base = { chunk.x * length + 6, chunk.y * length + y, chunk.z * length + 8 };
            int solid = 0;
            for (int x = -1; x <= 2; x++) for (int h = 0; h < 3; h++) for (int z = -2; z <= 3; z++) {
                solid += terrain_voxel_at(world, terrain, (int3) { base.x + x, base.y + h, base.z + z }) != 0;
            }
            if (solid) {
                continue;
            }
            zox_tst_dr_base = base;
            placed = 1;
        }
        if (!placed) {
            return zox_tst_dr_frames > 600;
        }
        const byte door = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockDoor)), BlockIndex);
        zox_tst_check(zox_tst_place_supported(world, terrain, realm, zox_tst_dr_base, door, 0), "the first door wasn't placed");
        const int3 second = { zox_tst_dr_base.x + 1, zox_tst_dr_base.y, zox_tst_dr_base.z };
        zox_tst_check(zox_tst_place_supported(world, terrain, realm, second, door, 0), "the second door wasn't placed");
        const float across[3] = { 0.1f, 0.5f, 0.9f };
        const float heights[2] = { 0.5f, 1.5f };
        for (int i = 0; i < zox_tst_dr_rays; i++) {
            const byte behind = i >= zox_tst_dr_rays / 2;
            const int j = i % (zox_tst_dr_rays / 2);
            const float3 at = float3_scale(float3_add(int3_to_float3(zox_tst_dr_base),
                (float3) { across[j % 3], heights[j / 3], behind ? 2.5f : -1.5f }), block);
            const entity e = zox_new();
            zox_set(e, Position3D, { at });
            zox_set(e, Rotation3D, { zox_tst_bact_facing(behind ? -1 : 1) });
            zox_set(e, TriggerActionB, { 0 });
            zox_set(e, RaycastRange, { 6 * block });
            zox_set(e, RaycastVoxelData, { 0 });
            zox_set_link(world, e, TerrainLink, terrain);
            zox_tst_dr_clickers[i] = e;
        }
        zox_tst_dr_stage = 1;
        zox_tst_dr_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the doors' block voxes and the rays";
    const entity vode = terrain_vode_at(world, terrain, zox_tst_dr_base);
    if (zox_tst_dr_frames < 10 || !zox_valid(vode) || !zox_has(vode, DoorSwing)) {
        return zox_tst_dr_frames > 600;
    }
    if (zox_tst_dr_stage == 1) {
        // every ray cast and hitting something first (a few frames after the block vox: it measured 12 misses), or 300
        //  frames on (a real miss)
        byte hit = 1;
        for (int i = 0; i < zox_tst_dr_rays; i++) {
            hit &= zox_get(zox_tst_dr_clickers[i], RaycastVoxelData)->result != 0;
        }
        if (!hit && zox_tst_dr_frames < 300) {
            return 0;
        }
        zox_tst_dr_measure(world, terrain, realm, 0);
        door_toggle(world, terrain, realm, zox_tst_dr_base);
        // open, it lies along its block's x 0 side (about its hinge, a block deep): the rays onto that line
        for (int i = 0; i < zox_tst_dr_rays; i++) {
            float3 at = zox_getv(zox_tst_dr_clickers[i], Position3D);
            at.x = (zox_tst_dr_base.x + 0.02f) * block;
            zox_setv(zox_tst_dr_clickers[i], Position3D, at);
        }
        zox_tst_dr_stage = 2;
        zox_tst_dr_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the door swinging open";
    if (zox_getv(vode, DoorSwing) < 1) {
        if (zox_tst_dr_frames > 600) {
            zox_tst_check(0, "facing its first way (no turn) the door never swung open, swing [%.2f]", zox_getv(vode, DoorSwing));
            return 1;
        }
        return 0;
    }
    zox_tst_dr_measure(world, terrain, realm, 1);
    for (int i = 0; i < zox_tst_dr_rays; i++) {
        zox_delete(zox_tst_dr_clickers[i]);
    }
    zox_tst_dr_stage = 3;
    return 1;
}

void zox_tst_door_rays_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_dr_stage == 3, "it stopped at stage [%i]", zox_tst_dr_stage);
    for (byte open = 0; open < 2; open++) {
        zox_log("[tst] door_rays %s: [%i] of [%i] rays found the door, [%i] another block, [%i] nothing", open ? "open" : "closed",
            zox_tst_dr_found[open], zox_tst_dr_rays, zox_tst_dr_other[open], zox_tst_dr_missed[open]);
        zox_tst_check(zox_tst_dr_found[open] == zox_tst_dr_rays, "%s, [%i] rays at the door found its neighbour, [%i] nothing",
            open ? "open" : "closed", zox_tst_dr_other[open], zox_tst_dr_missed[open]);
    }
}

void zox_tsts_door_rays(ecs* world) {
    zox_tst_add("door_rays", zox_tst_door_rays_ready, zox_tst_door_rays_check, 120);
}
