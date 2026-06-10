zox_sys2(HeightMapSystem) {
    // TODO: use terrains seed
    // TODO: use height frequency from biome maps
    const uint seed = global_seed;
    double height_frequency = 0.3; // terrain_frequency * 10;
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Chunk2Position);
    zox_sys_in(BiomeMap);
    zox_sys_out(HeightMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(Chunk2Position, tunk_position);
        zox_sys_i(BiomeMap, bmap);
        zox_sys_o(HeightMap, hmap);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        if (!bmap->length) {
            zox_logw("[hmap] biome map wasn't generated in time");
            continue;
        }
        // now generate hmap
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 map_size = int2_single(max_chunk_length);
        int2 position = int2_zero;
        if (!hmap->value) {
            initialize_HeightMap(hmap, map_size.x * map_size.y);
        }
        int2 position_start = (int2) {
            tunk_position->value.x * map_size.x,
            tunk_position->value.y * map_size.y
        };
        /*int2 mposition = (int2) { 0, 100 };
        double mradius = 80;
        double mheight = 8;*/
        int2 grid_position = position_start;
        for (position.x = 0; position.x < map_size.x; position.x++, grid_position.x++) {
            grid_position.y = position_start.y;
            for (position.y = 0; position.y < map_size.y; position.y++, grid_position.y++) {
                int index = int2_array_index(position, map_size);
                byte biome = bmap->value[index];
                // TODO: Use biome frequency
                // TODO: Blend frequency amongst several nearby ones
                double frequency = biome == 0 ? height_frequency : height_frequency * 2;
                // Mountain Test
                /*int mdistance = int2_distance(grid_position, mposition);
                double mmultiplier = 1;
                if (mdistance < mradius) {
                    mmultiplier = 1 + mheight * ((mradius - mdistance) / (float) mradius);
                }*/
                double perlin_value = perlin_octaves(
                    noise_positiver2 + (grid_position.x / ((float) max_chunk_length)),
                    noise_positiver2 + (grid_position.y / ((float) max_chunk_length)),
                    frequency,
                    seed,
                    terrain_octaves
                );
                int value = int_floorf(perlin_value * max_chunk_length);
                // hmap->value[index] = int_clamp(value, 0, 255);
                hmap->value[index] = int_clamp(value, 0, render_distance_y * max_chunk_length - 1);
            }
        }
    }
} zox_sys_end(HeightMapSystem);
