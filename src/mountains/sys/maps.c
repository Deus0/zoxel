// NOTE: Adds height to the heightmap using region mountains
zox_sys2(MountainMapSystem) {
    // uint seed = global_seed;
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(RegionLink);
    zox_sys_in(TunkPosition);
    zox_sys_out(VegetationMap);
    zox_sys_out(HeightMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(RegionLink, region);
        zox_sys_i(TunkPosition, tunk_position);
        zox_sys_o(VegetationMap, vegetation_map);
        zox_sys_o(HeightMap, height_map);
        // NOTE: Runs after heights system
        if (generate->value != zox_dirty_end) {
            continue;
        }
        if (!height_map->length) {
            zox_logw("Invalid [height_map] in MountainMapSystem");
            continue;
        }
        if (!vegetation_map->length) {
            zox_logw("Invalid [vegetation_map] in MountainMapSystem");
            continue;
        }
        if (!zox_valid(region->value)) {
            zox_loge("Tunk has invalid region at [%ix%i]", tunk_position->value.x, tunk_position->value.y);
            continue;
        }
        // now generate height_map
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 map_size = int2_single(max_chunk_length);
        int2 position_start = (int2) { tunk_position->value.x * map_size.x,  tunk_position->value.y * map_size.y };
        entity mountains[zox_children_capacity];
        uint mountains_length = zox_get_children_by_id(world, region->value, mountains, zox_children_capacity, zox_id(Mountain));
        if (dbg_log >= 2) {
            zox_log("Towns found in Tunk [%ix%i]: [%i]", tunk_position->value.x, tunk_position->value.y, mountains_length);
        }
        for (int j = 0; j < mountains_length; j++) {
            entity e2 = mountains[j];
            int2 mountain_position = zox_getv(e2, BlockPosition2);
            byte mountain_radius = zox_getv(e2, Radius);
            byte mountain_height = zox_getv(e2, Height);
            int2 grid_position = position_start;
            int2 position = int2_zero;
            for (position.x = 0; position.x < map_size.x; position.x++, grid_position.x++) {
                grid_position.y = position_start.y;
                for (position.y = 0; position.y < map_size.y; position.y++, grid_position.y++) {
                    int mountain_distance = int2_distance(grid_position, mountain_position);
                    if (mountain_distance > mountain_radius) {
                        continue;
                    }
                    int index = int2_array_index(position, map_size);
                    double mountain_multiplier = 1 + mountain_height * ((mountain_radius - mountain_distance) / (float) mountain_radius);
                    int value = height_map->value[index];
                    value *= mountain_multiplier;
                    height_map->value[index] = int_clamp(value, 0, render_distance_y * max_chunk_length - 1);
                    // Clear Vegetation for middle of mountains
                    if (mountain_distance < 2 / (mountain_radius * 3)) {
                        vegetation_map->value[index] = 0;
                    }
                }
            }
        }
    }
} zox_sys_end(MountainMapSystem);
