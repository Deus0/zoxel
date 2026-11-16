// tst: ground_removed, grass and a flower go when the block under them does (items/fun/multi_block.c,
//  block_needs_ground), and drop their items as breaking them does
//  - placed on ground (written under them), the ground under each removed: they're air a few frames after
//  - a solid block on ground stays when its ground goes (only what stands on it falls)
//  - run: bash bsh/tst.sh ground_removed
byte zox_tst_ground_stage = 0;
int zox_tst_ground_frames = 0;
int3 zox_tst_ground_cells[3];   // grass, a flower, a solid block

byte zox_tst_ground_removed_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    zox_tst_ground_frames++;
    zox_tst_progress(zox_tst_ground_stage * 1000 + zox_tst_ground_frames);
    if (zox_tst_ground_stage == 0) {
        zox_tst_waiting = "air for grass, a flower and a block";
        const float scale = zox_getv(terrain, BlockScale);
        const int length = octree_size(zox_getv(terrain, NodeDepth));
        const int3 chunk = real_position_to_chunk_position(zox_getv(character, Position3D), length, scale);
        const int3 origin = { chunk.x * length, chunk.y * length, chunk.z * length };
        for (int y = 1; y < length * 2 - 2; y++) {
            int solid = 0;
            for (int i = 0; i < 3; i++) {
                solid += terrain_voxel_at(world, terrain, (int3) { origin.x + 4 + i * 3, origin.y + y, origin.z + 6 }) != 0;
            }
            if (solid) {
                continue;
            }
            const byte values[3] = { zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockGrass)), BlockIndex),
                zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockFlower)), BlockIndex), zox_tst_ground_voxel(world, realm) };
            for (int i = 0; i < 3; i++) {
                zox_tst_ground_cells[i] = (int3) { origin.x + 4 + i * 3, origin.y + y, origin.z + 6 };
                zox_tst_check(zox_tst_place_supported(world, terrain, realm, zox_tst_ground_cells[i], values[i], 0), "block [%i] wasn't placed", i);
            }
            zox_tst_ground_stage = 1;
            zox_tst_ground_frames = 0;
            return 0;
        }
        return zox_tst_ground_frames > 600;
    }
    if (zox_tst_ground_frames < 10) {
        return 0;
    }
    if (zox_tst_ground_stage == 1) {
        for (int i = 0; i < 3; i++) {
            zox_tst_check(terrain_voxel_at(world, terrain, zox_tst_ground_cells[i]) > 0, "block [%i] isn't there", i);
            const int3 c = zox_tst_ground_cells[i];
            terrain_queue_voxel(world, terrain, (int3) { c.x, c.y - 1, c.z }, 0, 0);
        }
        zox_tst_ground_stage = 2;
        zox_tst_ground_frames = 0;
        return 0;
    }
    const char* names[3] = { "grass", "the flower", "the solid block" };
    for (int i = 0; i < 3; i++) {
        const int voxel = terrain_voxel_at(world, terrain, zox_tst_ground_cells[i]);
        zox_log("[tst] %s after its ground went: [%i]", names[i], voxel);
        zox_tst_check(i < 2 ? voxel == 0 : voxel > 0, "%s is [%i] after its ground went", names[i], voxel);
    }
    zox_tst_ground_stage = 3;
    return 1;
}

void zox_tst_ground_removed_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_ground_stage == 3, "it stopped at stage [%i]", zox_tst_ground_stage);
}

void zox_tsts_ground_removed(ecs* world) {
    zox_tst_add("ground_removed", zox_tst_ground_removed_ready, zox_tst_ground_removed_check, 120);
}
