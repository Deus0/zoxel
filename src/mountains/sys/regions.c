// NOTE: Returns 1 if successfully added a new position/radius
byte find_position_in_bounds(int2 region_position, int2 region_size, byte minimum_radius, byte maximum_radius, int2* positions, byte* radii, byte added) {
    const int max_attempts = 100;
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        byte new_radius = rand_range(minimum_radius, maximum_radius);
        int2 new_position = {
            rand_range(
                region_position.x + new_radius,
                region_position.x + region_size.x - new_radius),
                rand_range(
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
    byte max_size = 99;
    byte min_height = 4;
    byte max_height = 16;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(RegionPosition);
    zox_sys_in(BlockPosition);
    zox_sys_in(BlockSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(RegionPosition, position);
        zox_sys_i(BlockPosition, voxel_position);
        zox_sys_i(BlockSize, size);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        byte spawn_count = rand_range(1, 9);
        if (dbg_log) {
            zox_log("[%s] Is Spawning [%i] Mountains", zox_get_name(e), spawn_count);
        }
        int2 region_voxel_position = (int2) { voxel_position->value.x, voxel_position->value.z };
        int2 region_voxel_size = (int2) { size->value.x, size->value.z };
        int2 positions[spawn_count];
        byte sizes[spawn_count];
        for (int j = 0; j < spawn_count; j++) {
            if (!find_position_in_bounds(region_voxel_position, region_voxel_size, min_size, max_size, positions, sizes, j)) {
                continue;
            }
            int2 spawn_position = positions[j];
            byte radius = sizes[j];
            byte height = rand_range(min_height, max_height);
            spawn_mountain(world, prefab_mountain, e, spawn_position, radius, height);
            if (dbg_log) {
                zox_log("   ++ Mountain [%ix%i]", spawn_position.x, spawn_position.y);
            }
        }
    }
} zox_sys_end(RegionMountainSystem);
