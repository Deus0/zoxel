// TODO: use terrains seed
// TODO: use height frequency from biome maps
// TODO: Check Sand Height in this system

zox_sys2(VegetationMapSystem) {
    uint seed = global_seed;
    double veggie_frequency = 0.6;
    double veggie_amplitude = 1.0;
    byte veggie_octaves = 12;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(TunkPosition);
    zox_sys_in(BiomeMap);
    zox_sys_out(VegetationMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(TunkPosition, cposition);
        zox_sys_i(BiomeMap, bmap);
        zox_sys_o(VegetationMap, vmap);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        if (!bmap->length) {
            zox_logw("[vmap] bmap map wasn't generated in time");
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        zox_geter_value(terrain, RealmLink, entity, realm);
        if (!zox_valid(realm)) {
            continue;
        }
        zox_geter(realm, BiomeLinks, realm_biomes);
        if (!realm_biomes->length) {
            zox_log_error("No Biomes on Realm");
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
        entity biome;
        float grass_chance;
        float weeds_chance;
        float tree_chance;
        int2 gposition = gposition_start;
        for (lposition.x = 0; lposition.x < hsize.x; lposition.x++, gposition.x++) {
            gposition.y = gposition_start.y;
            for (lposition.y = 0; lposition.y < hsize.y; lposition.y++, gposition.y++) {
                int index = int2_array_index(lposition, hsize);
                // Get Biome Data
                byte biome_id = bmap->value[index];
                if (biome_id >= realm_biomes->length) {
                    zox_loge("Biome ID OOB [%i] of [%i]", biome_id, realm_biomes->length);
                    continue;
                }
                entity new_biome = realm_biomes->value[biome_id];
                if (!zox_valid(new_biome)) {
                    zox_loge("Biome is invalid [%i]", biome_id);
                    continue;
                }
                // NOTE: Updates our cache of our biome
                if (biome != new_biome) {
                    biome = new_biome;
                    grass_chance = zox_getv(biome, GrassChance);
                    weeds_chance = zox_getv(biome, WeedsChance);
                    tree_chance = zox_getv(biome, TreeChance);
                }
                double perlin_value = veggie_amplitude * perlin_octaves(
                    noise_positiver2 + (gposition.x / ((float) max_chunk_length)),
                    noise_positiver2 + (gposition.y / ((float) max_chunk_length)),
                    veggie_frequency,
                    seed, veggie_octaves);
                byte value;
                if (perlin_value >= tree_chance) {
                    value = 3;  // Trees
                } else if (perlin_value >= weeds_chance) {
                    value = 2;  // Weeds
                } else if (perlin_value >= grass_chance) {
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
