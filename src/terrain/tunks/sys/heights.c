zox_sys2(HeightMapSystem) {
    // TODO: use terrains seed
    // TODO: use height frequency from biome maps
    const uint seed = global_seed;
    double height_frequency = terrain_frequency;
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Chunk2Position);
    zox_sys_out(HeightMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(Chunk2Position, cposition);
        zox_sys_o(HeightMap, heights);

        if (generate->value != zox_dirty_active) {
            continue;
        }

        // now generate heights
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 hsize = int2_single(max_chunk_length);

        int2 lposition = int2_zero;
        if (!heights->value) {
            initialize_HeightMap(heights, hsize.x * hsize.y);
        }

        //byte is_mountain = 0;
        //double height_frequency = is_mountain ? terrain_frequency * mountain_amplifier : terrain_frequency;

        int2 gposition_start = (int2) {
            cposition->value.x * hsize.x,
            cposition->value.y * hsize.y
        };

        int2 gposition = gposition_start;
        for (lposition.x = 0; lposition.x < hsize.x; lposition.x++, gposition.x++) {

            gposition.y = gposition_start.y;
            for (lposition.y = 0; lposition.y < hsize.y; lposition.y++, gposition.y++) {

                double perlin_value = perlin_terrain(
                    noise_positiver2 + (gposition.x / ((float) max_chunk_length)),
                    noise_positiver2 + (gposition.y / ((float) max_chunk_length)),
                    height_frequency,
                    seed,
                    terrain_octaves
                );

                int valuei = int_floorf(perlin_value * max_chunk_length);
                byte value = 128 + valuei;

                // debug value
                /*value = 128 + (int_abs(cposition->value.x * 2) + int_abs(cposition->value.y * 2));
                if (lposition.x >= hsize.x / 2) value += 1;
                if (lposition.y >= hsize.y / 2) value += 1;*/

                int index = int2_array_index(lposition, hsize);
                heights->value[index] = value;

                // TODO: Print this out onto texture

                // zox_log("- TerrainHeight [%ix%i] [%i] [%f]", gposition.x, gposition.y, value, perlin_value);

                //perlin_value *= height_amplifier;
                //const int global_position_y = int_floorf(+ perlin_value);
            }
        }
    }
} zox_sys_end(HeightMapSystem);