zox_sys2(HeightMapSystem) {
    // TODO: use terrains seed
    // TODO: use height frequency from biome maps
    const uint seed = global_seed;
    double height_frequency = 0.3; // terrain_frequency * 10;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Chunk2Position);
    zox_sys_in(BiomeMap);
    zox_sys_out(HeightMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(Chunk2Position, tunk_position);
        zox_sys_i(BiomeMap, biome_map);
        zox_sys_o(HeightMap, height_map);
        if (generate->value != zox_dirty_end) { // != zox_dirty_active) {
            continue;
        }
        if (!biome_map->length || !biome_map->value) {
            zox_logw("[biome_map] biome map wasn't generated in time");
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        // now generate height_map
        int max_chunk_length = powers_of_two[terrain_depth];
        byte max_height = int_min(render_distance_y * max_chunk_length - 1, 255);
        int2 map_size = int2_single(max_chunk_length);
        if (biome_map->length != map_size.x * map_size.y) {
            zox_logw("[biome_map] invalid size");
            continue;
        }
        if (!height_map->value) {
            initialize_HeightMap(height_map, map_size.x * map_size.y);
        }
        int2 position_start = (int2) {
            tunk_position->value.x * map_size.x,
            tunk_position->value.y * map_size.y
        };
        int2 grid_position = position_start;
        int2 position = int2_zero;
        for (position.x = 0; position.x < map_size.x; position.x++, grid_position.x++) {
            grid_position.y = position_start.y;
            for (position.y = 0; position.y < map_size.y; position.y++, grid_position.y++) {
                int index = int2_array_index(position, map_size);
                byte biome_id = biome_map->value[index];
                // TODO: Use biome frequency
                // TODO: Blend frequency amongst several nearby ones
                double frequency = biome_id == 0 ? height_frequency : height_frequency * 2;
                //  double frequency = height_frequency;
                double perlin_value = perlin_octaves(
                    noise_positiver2 + (grid_position.x / ((float) max_chunk_length)),
                    noise_positiver2 + (grid_position.y / ((float) max_chunk_length)),
                    frequency,
                    seed,
                    terrain_octaves
                );
                int value = int_floorf(perlin_value * max_chunk_length);
                height_map->value[index] = int_clamp(value, 0, max_height);
            }
        }
    }
} zox_sys_end(HeightMapSystem);
