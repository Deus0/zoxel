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
        zox_sys_i(Chunk2Position, cposition);
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
        int2 hsize = int2_single(max_chunk_length);
        int2 lposition = int2_zero;
        if (!hmap->value) {
            initialize_HeightMap(hmap, hsize.x * hsize.y);
        }
        int2 gposition_start = (int2) {
            cposition->value.x * hsize.x,
            cposition->value.y * hsize.y
        };
        int2 mposition = (int2) { 0, 100 };
        double mradius = 80;
        double mheight = 8;
        int2 gposition = gposition_start;
        for (lposition.x = 0; lposition.x < hsize.x; lposition.x++, gposition.x++) {
            gposition.y = gposition_start.y;
            for (lposition.y = 0; lposition.y < hsize.y; lposition.y++, gposition.y++) {
                int index = int2_array_index(lposition, hsize);
                byte biome = bmap->value[index];
                // TODO: Use biome frequency
                // TODO: Blend frequency amongst several nearby ones
                double frequency = biome == 0 ? height_frequency : height_frequency * 2;
                // Mountain Test
                int mdistance = int2_distance(gposition, mposition);
                double mmultiplier = 1;
                if (mdistance < mradius) {
                    mmultiplier = 1 + mheight * ((mradius - mdistance) / (float) mradius);
                }
                double perlin_value = perlin_octaves(
                    noise_positiver2 + (gposition.x / ((float) max_chunk_length)),
                    noise_positiver2 + (gposition.y / ((float) max_chunk_length)),
                    frequency,
                    seed,
                    terrain_octaves
                );
                int value = int_floorf(perlin_value * max_chunk_length * mmultiplier);
                hmap->value[index] = int_clamp(value, 0, 255);
                hmap->value[index] = int_clamp(hmap->value[index], 0, render_distance_y * max_chunk_length - 1);
            }
        }
    }
} zox_sys_end(HeightMapSystem);
