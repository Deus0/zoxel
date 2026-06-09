// NOTE: Returns 1 if successfully added a new position/size
byte place_new_town(int2 region_position, int2 region_size, int2 minimum_size, int2 maximum_size, int2* positions, int2* sizes, byte added) {
    const int max_attempts = 100;
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        int2 new_size = {
            rand_range(minimum_size.x, maximum_size.x),
            rand_range(minimum_size.y, maximum_size.y)
        };
        int2 new_position = {
            rand_range(
                region_position.x,
                region_position.x + region_size.x - new_size.x),
                rand_range(
                    region_position.y,
                    region_position.y + region_size.y - new_size.y)
        };
        byte valid = 1;
        for (int i = 0; i < added; i++) {
            int2 p = positions[i];
            int2 s = sizes[i];
            if (new_position.x < p.x + s.x &&
                new_position.x + new_size.x > p.x &&
                new_position.y < p.y + s.y &&
                new_position.y + new_size.y > p.y)
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

// NOTE: Spawns X Towns per region
zox_sys2(RegionTownsSystem) {
    byte dbg_log = 0;
    int2 min_size = (int2) { 32, 32 };
    int2 max_size = (int2) { 180, 180 };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(RegionPosition);
    zox_sys_in(VoxelPosition);
    zox_sys_in(VoxelSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(RegionPosition, position);
        zox_sys_i(VoxelPosition, voxel_position);
        zox_sys_i(VoxelSize, size);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        byte spawn_count = rand_range(1, 9);
        if (dbg_log) {
            zox_log("[%s] Is Spawning [%i] Towns", zox_get_name(e), spawn_count);
        }
        int2 region_voxel_position = (int2) { voxel_position->value.x, voxel_position->value.z };
        int2 region_voxel_size = (int2) { size->value.x, size->value.z };
        int2 positions[spawn_count];
        int2 sizes[spawn_count];
        for (int j = 0; j < spawn_count; j++) {
            if (!place_new_town(region_voxel_position, region_voxel_size, min_size, max_size, positions, sizes, j)) {
                continue;
            }
            int2 spawn_position = positions[j];
            int2 spawn_size = positions[j];
            // int2 spawn_position = (int2) { voxel_position->value.x + size->value.x / 2, voxel_position->value.z + size->value.z / 2 };
            spawn_town(world, prefab_town, e, spawn_position, spawn_size);
            if (dbg_log) {
                zox_log("   + New Town [%ix%i]", spawn_position.x, spawn_position.y);
            }
        }
    }
} zox_sys_end(RegionTownsSystem);
