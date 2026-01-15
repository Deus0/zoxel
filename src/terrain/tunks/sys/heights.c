zox_sys2(HeightMapSystem) {
    const uint seed = global_seed;  // TODO: use terrains seed
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
        int2 lposition = int2_zero;
        int2 size = (int2) { 32, 32 };
        if (!heights->value) {
            initialize_HeightMap(heights, size.x * size.y);
        }

        //byte is_mountain = 0;
        //double height_frequency = is_mountain ? terrain_frequency * mountain_amplifier : terrain_frequency;

        double height_frequency = 0.007216; // terrain_frequency;
        int2 gposition = (int2) {
            cposition->value.x * size.x,
            cposition->value.y * size.y
        };

        for (lposition.x = 0; lposition.x < size.x; lposition.x++, gposition.x++) {
            for (lposition.y = 0; lposition.y < size.y; lposition.y++, gposition.y++) {
                int index = int2_array_index(lposition, size);

                double perlin_value = perlin_terrain(
                    noise_positiver2 + gposition.x / 32.0f,
                    noise_positiver2 + gposition.y / 32.0f,
                    height_frequency,
                    seed,
                    terrain_octaves
                );
                byte value = 64 + ((int) perlin_value); //rand() % 32;
                heights->value[index] = value;

                // TODO: Print this out onto texture

                // zox_log("- TerrainHeight [%ix%i] [%i] [%f]", gposition.x, gposition.y, value, perlin_value);

                //perlin_value *= height_amplifier;
                //const int global_position_y = int_floorf(+ perlin_value);
            }
        }

        // zox_log("Generated Heightmap at [%ix%i]", cposition->value.x, cposition->value.y);

    }
} zox_sys_end(HeightMapSystem);