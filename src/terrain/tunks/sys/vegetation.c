zox_sys2(VegetationMapSystem) {
    // TODO: use terrains seed
    // TODO: use height frequency from biome maps
    uint seed = global_seed;
    double veggie_frequency = 0.6;
    double veggie_amplitude = 1.0;
    byte veggie_octaves = 12;

    double grass_cutoff_0 = 0.52;
    double weeds_cutoff_0 = 0.59;

    double grass_cutoff_1 = 0.62;
    double weeds_cutoff_1 = 0.74;

    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Chunk2Position);
    zox_sys_in(BiomeMap);
    zox_sys_out(VegetationMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(Chunk2Position, cposition);
        zox_sys_i(BiomeMap, bmap);
        zox_sys_o(VegetationMap, vmap);

        if (generate->value != zox_dirty_active) {
            continue;
        }

        if (!bmap->length) {
            zox_logw("[vmap] bmap map wasn't generated in time");
            continue;
        }

        // now generate heights
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 hsize = int2_single(max_chunk_length);

        int2 lposition = int2_zero;
        if (!vmap->value) {
            initialize_VegetationMap(vmap, hsize.x * hsize.y);
        }

        int2 gposition_start = (int2) {
            cposition->value.x * hsize.x,
            cposition->value.y * hsize.y
        };

        int2 gposition = gposition_start;
        for (lposition.x = 0; lposition.x < hsize.x; lposition.x++, gposition.x++) {
            gposition.y = gposition_start.y;
            for (lposition.y = 0; lposition.y < hsize.y; lposition.y++, gposition.y++) {

                int index = int2_array_index(lposition, hsize);

                // Get Biome Data
                byte biome = bmap->value[index];
                double frequency = biome == 0 ? veggie_frequency * 2 : veggie_frequency;
                double grass_cutoff = biome == 0 ? grass_cutoff_0 : grass_cutoff_1;
                double weeds_cutoff = biome == 0 ? weeds_cutoff_0 : weeds_cutoff_1;


                double pvalue = veggie_amplitude * perlin_octaves(
                    noise_positiver2 + (gposition.x / ((float) max_chunk_length)),
                    noise_positiver2 + (gposition.y / ((float) max_chunk_length)),
                    frequency,
                    seed,
                    veggie_octaves
                );

                byte value;

                if (pvalue >= weeds_cutoff) {
                    value = 2;  // Weeds
                } else if (pvalue >= grass_cutoff) {
                    value = 1;  // Grass
                } else {
                    value = 0;  // dirt
                }

                vmap->value[index] = value;

                // zox_log("value veggie: %f", value);
            }
        }
    }
} zox_sys_end(VegetationMapSystem);