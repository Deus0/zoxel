// NOTE: Returns 1 if successfully added a new position/radius
byte find_position_in_bounds(lint seed, int2 region_position, int2 region_size, byte minimum_radius, byte maximum_radius, int2* positions, byte* radii, byte added) {
    const int max_attempts = 100;
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        lint seed2 = seed + attempt * 100;
        byte new_radius = seed_range(seed2 + 0, minimum_radius, maximum_radius);
        int2 new_position = {
            seed_range(seed2 + 1,
                region_position.x + new_radius,
                region_position.x + region_size.x - new_radius),
            seed_range(seed2 + 2,
                region_position.y + new_radius,
                region_position.y + region_size.y - new_radius)
        };
        byte valid = 1;
        for (int i = 0; i < added; i++) {
            int2 p = positions[i];
            int r = radii[i];
            int dx = new_position.x - p.x;
            int dy = new_position.y - p.y;
            int min_distance = new_radius + r;
            if (dx * dx + dy * dy < min_distance * min_distance) {
                valid = 0;
                break;
            }
        }
        if (valid) {
            positions[added] = new_position;
            radii[added] = new_radius;
            return 1;
        }
    }
    return 0;
}

// NOTE: Spawns X Towns per region
zox_sys2(RegionMountainSystem) {
    byte dbg_log = 0;
    byte min_size = 24;
    byte max_size = 128;
    byte min_height = 3;
    byte max_height = 16;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Seed);
    zox_sys_in(BlockPosition2);
    zox_sys_in(BlockSize2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(Seed, seed);
        zox_sys_i(BlockPosition2, block_position);
        zox_sys_i(BlockSize2, block_size);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        byte spawn_count = rand_range(1, 9);
        if (dbg_log) {
            zox_log("[%s] Is Spawning [%i] Mountains", zox_get_name(e), spawn_count);
        }
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        // int2 region_block_position = region_position_to_block_position2(position->value, terrain_depth);
        if (dbg_log) {
            zox_log("   - Region Position [%ix%i]", block_position->value.x, block_position->value.y);
        };
        int2 positions[spawn_count];
        byte sizes[spawn_count];
        for (int j = 0; j < spawn_count; j++) {
            if (!find_position_in_bounds(seed->value, block_position->value, block_size->value, min_size, max_size, positions, sizes, j)) {
                continue;
            }
            int2 mountain_position = positions[j];
            byte radius = sizes[j];
            byte height = seed_range(seed->value + j, min_height, max_height);
            // mountain_position = int2_add(mountain_position, spawn_position);
            lint mountain_seed = position_seed2(seed->value, mountain_position);
            spawn_mountain(world, prefab_mountain, e, mountain_seed, mountain_position, radius, height);
            if (dbg_log) {
                zox_log("   ++ Mountain [%ix%i] H [%i] R [%i]", mountain_position.x, mountain_position.y, height, radius);
            }
        }
    }
} zox_sys_end(RegionMountainSystem);
