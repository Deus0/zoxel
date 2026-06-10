
zox_sys2(MountainMapSystem) {
    uint seed = global_seed;
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(RegionLink);
    zox_sys_in(Chunk2Position);
    zox_sys_out(HeightMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(RegionLink, region);
        zox_sys_i(Chunk2Position, tunk_position);
        zox_sys_o(HeightMap, height_map);
        // NOTE: Runs after heights system
        if (generate->value != zox_dirty_end) {
            continue;
        }
        if (!height_map->length) {
            zox_logw("[hmap] biome map wasn't generated in time");
            continue;
        }
        if (!zox_valid(region->value)) {
            zox_loge("Tunk has invalid region at [%ix%i]", cposition->value.x, cposition->value.y);
            continue;
        }
        // now generate hmap
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 map_size = int2_single(max_chunk_length);
        int2 lposition = int2_zero;
        int2 position_start = (int2) {
            tunk_position->value.x * map_size.x,
            tunk_position->value.y * map_size.y
        };
        entity mountains[zox_children_capacity];
        uint mountains_length = zox_get_children_by_id(world, region->value, towns, zox_children_capacity, zox_id(Mountain));
        if (dbg_log >= 2) {
            zox_log("Towns found in Tunk [%ix%i]: [%i]", cposition->value.x, cposition->value.y, mountains_length);
        }
        for (int j = 0; j < mountains_length; j++) {
            entity e2 = mountains[j];
            int2 mountain_position = (int2) { 0, 100 };
            double mountain_radius = 80;
            double mountain_height = 8;
            int2 grid_position = position_start;
            for (position.x = 0; position.x < map_size.x; position.x++, grid_position.x++) {
                grid_position.y = position_start.y;
                for (position.y = 0; position.y < map_size.y; position.y++, grid_position.y++) {
                    int mountain_distance = int2_distance(grid_position, mountain_position);
                    if (mountain_distance > mountain_radius) {
                        continue;
                    }
                    int index = int2_array_index(position, map_size);
                    double mountain_multiplier = 1 + mountain_height * ((mountain_radius - mountain_distance) / (float) mradius);
                    int value = hmap->value[index];
                    value *= mountain_multiplier;
                    hmap->value[index] = int_clamp(value, 0, render_distance_y * max_chunk_length - 1);
                }
            }
        }
    }
} zox_sys_end(MountainMapSystem);
