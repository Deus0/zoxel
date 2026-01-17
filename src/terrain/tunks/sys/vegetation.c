zox_sys2(VegetationMapSystem) {
    // TODO: use terrains seed
    // TODO: use height frequency from biome maps
    const uint seed = global_seed;
    double veggie_frequency = 3.0;
    int veggie_octaves = 8;
    double veggie_amplitude = 1.0;
    double grass_cutoff = 0.66;
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
                byte biome = bmap->value[index];

                double frequency = biome == 0 ? veggie_frequency : veggie_frequency * 2;

                double value = veggie_amplitude * perlin_octaves(
                    noise_positiver2 + (gposition.x / ((float) max_chunk_length)),
                    noise_positiver2 + (gposition.y / ((float) max_chunk_length)),
                    frequency,
                    seed,
                    veggie_octaves
                );
                // Maps value to 1 for grass top, 2 for place grass
                vmap->value[index] = value >= grass_cutoff ? 2 : ( value >= 0.1 ? 1 : 0);

                // zox_log("value veggie: %f", value);
            }
        }
    }
} zox_sys_end(VegetationMapSystem);