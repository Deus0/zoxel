// NOTE: Returns 1 if successfully added a new position/size
byte place_new_town(lint seed, int2 region_position, int2 region_size, byte2 padding, byte2 minimum_size, byte2 maximum_size, int2* positions, byte2* sizes, byte added) {
    int max_attempts = 100;
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        lint seed2 = seed + attempt * 100;
        byte2 new_size = {
            seed_range(seed2 + 0, minimum_size.x, maximum_size.x),
            seed_range(seed2 + 1, minimum_size.y, maximum_size.y)
        };
        int2 new_position = {
            seed_range(seed2 + 2,
                region_position.x + padding.x + new_size.x / 2,
                region_position.x + region_size.x - padding.x - new_size.x / 2),
            seed_range(seed2 + 3,
                region_position.y + padding.y + new_size.x / 2,
                region_position.y + region_size.y - padding.y - new_size.y / 2)
        };
        byte valid = 1;
        for (int i = 0; i < added; i++) {
            int2 p = positions[i];
            byte2 s = sizes[i];
            if (new_position.x - new_size.x / 2 < p.x + s.x / 2 &&
                new_position.x + new_size.x / 2 > p.x - s.x / 2 &&
                new_position.y - new_size.y / 2 < p.y + s.y / 2 &&
                new_position.y + new_size.y / 2> p.y - s.y / 2)
            {
                valid = 0;
                break;
            }
        }
        if (valid) {
            positions[added] = new_position;
            sizes[added] = new_size;
            return 1;
        }
    }
    return 0;
}

// TODO: Add Region Position to Town Positions
// NOTE: Spawns X Towns per region
zox_sys2(RegionTownsSystem) {
    byte dbg_log = 0;
    byte2 min_size = (byte2) { 22, 22 };
    byte2 max_size = (byte2) { 42, 42 };
    byte2 wall_height_range = (byte2) { 2, 6 };
    byte2 wall_thickness_range = (byte2) { 1, 4 };
    byte2 padding = byte2_single(8);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Seed);
    zox_sys_in(RegionPosition);
    zox_sys_in(BlockPosition2);
    zox_sys_in(BlockSize2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(Seed, seed);
        zox_sys_i(RegionPosition, position);
        zox_sys_i(BlockPosition2, block_position);
        zox_sys_i(BlockSize2, block_size);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        byte spawn_count = rand_range(1, 7);
        if (dbg_log) {
            zox_log("[%s] Is Spawning [%i] Towns", zox_get_name(e), spawn_count);
        };
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        int2 region_block_position = region_position_to_block_position2(position->value, terrain_depth);
        if (dbg_log) {
            zox_log("   - Region Position [%ix%i]", region_block_position.x, region_block_position.y);
        };
        int2 positions[spawn_count];
        byte2 sizes[spawn_count];
        for (int j = 0; j < spawn_count; j++) {
            if (!place_new_town(seed->value, block_position->value, block_size->value, padding, min_size, max_size, positions, sizes, j)) {
                continue;
            }
            int2 spawn_position = positions[j];
            byte2 spawn_size = sizes[j];
            byte wall_height = seed_range(seed->value, wall_height_range.x, wall_height_range.y);
            byte wall_thickness = rand_range(wall_thickness_range.x, wall_thickness_range.y);
            spawn_position = int2_add(region_block_position, spawn_position);
            lint town_seed = position_seed2(seed->value, spawn_position);
            spawn_town(world, prefab_town, e, town_seed, spawn_position, spawn_size, wall_height, wall_thickness);
            if (dbg_log) {
                zox_log("   + New Town [%ix%i] Size [%ix%i]", spawn_position.x, spawn_position.y, spawn_size.x, spawn_size.y);
            }
        }
    }
} zox_sys_end(RegionTownsSystem);
