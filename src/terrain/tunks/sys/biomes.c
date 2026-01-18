// Generates a Biome Map
zox_sys2(BiomeMapSystem) {
    const uint seed = global_seed;
    double biome_frequency = 0.04;
    byte biome_octaves = 2;
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Chunk2Position);
    zox_sys_out(BiomeMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(Chunk2Position, cposition);
        zox_sys_o(BiomeMap, biomes);

        if (generate->value != zox_dirty_active) {
            continue;
        }

        // now generate heights
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 hsize = int2_single(max_chunk_length);

        int2 lposition = int2_zero;
        if (!biomes->value) {
            initialize_BiomeMap(biomes, hsize.x * hsize.y);
        }

        int2 gposition_start = (int2) {
            cposition->value.x * hsize.x,
            cposition->value.y * hsize.y
        };
        int2 gposition = gposition_start;

        for (lposition.x = 0; lposition.x < hsize.x; lposition.x++, gposition.x++) {
            gposition.y = gposition_start.y;
            for (lposition.y = 0; lposition.y < hsize.y; lposition.y++, gposition.y++) {

                double perlin_value = perlin_octaves(
                    noise_positiver2 + (gposition.x / ((float) max_chunk_length)),
                    noise_positiver2 + (gposition.y / ((float) max_chunk_length)),
                    biome_frequency,
                    seed,
                    biome_octaves
                );

                byte value = perlin_value > 0.5 ? 1 : 0;

                /*byte value = 0;
                if (cposition->value.x > 0) {
                    value = 1;
                } else {
                    value = 0;
                }*/

                int index = int2_array_index(lposition, hsize);
                biomes->value[index] = value;

                // zox_log("biome value [%i]", value);
            }
        }
    }
} zox_sys_end(BiomeMapSystem);