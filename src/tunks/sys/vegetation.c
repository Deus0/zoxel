// TODO: use terrains seed
// TODO: use height frequency from biome maps
// TODO: Check Sand Height in this system
typedef struct {
    float perlin;
    byte value;
} place_chance;

// NOTE: Uses nearest algorithm for the vegetation maps
static inline byte choose_place(const place_chance *places, byte count, double perlin_value) {
    byte best = places[0].value;
    double best_dist = fabs(perlin_value - places[0].perlin);
    for (size_t i = 1; i < count; ++i) {
        double dist = fabs(perlin_value - places[i].perlin);
        if (dist < best_dist) {
            best_dist = dist;
            best = places[i].value;
        }
    }
    return best;
}

zox_sys2(VegetationMapSystem) {
    uint seed = global_seed;
    double veggie_frequency = 0.6;
    double veggie_amplitude = 1.0;
    byte veggie_octaves = 12;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkPosition);
    zox_sys_in(BiomeMap);
    zox_sys_out(GenerateTunk);
    zox_sys_out(VegetationMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkPosition, cposition);
        zox_sys_i(BiomeMap, bmap);
        zox_sys_o(GenerateTunk, generate);
        zox_sys_o(VegetationMap, vmap);
        if (generate->value != zox_generate_tunk_vegetation) {
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
        int max_chunk_length = octree_size(terrain_depth);
        int2 hsize = int2_single(max_chunk_length);
        int2 lposition = int2_zero;
        if (!vmap->value) {
            initialize_VegetationMap(vmap, hsize.x * hsize.y);
        }
        int2 gposition_start = (int2) {
            cposition->value.x * hsize.x,
            cposition->value.y * hsize.y
        };
        entity biome = 0;
        byte places_count = 5;
        place_chance places[places_count];
        memset(places, 0, sizeof(places));
        places[0].value = zox_vegetation_dirt;
        places[1].value = zox_vegetation_grass;
        places[2].value = zox_vegetation_weeds;
        places[3].value = zox_vegetation_trees;
        places[4].value = zox_vegetation_flowers;
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
                    // grass_chance
                    places[0].perlin = zox_getv(biome, DirtChance);
                    places[1].perlin = zox_getv(biome, GrassChance);
                    places[2].perlin = zox_getv(biome, WeedsChance);
                    places[3].perlin = zox_getv(biome, TreeChance);
                    places[4].perlin = zox_getv(biome, FlowerChance);
                }
                double perlin_value = veggie_amplitude * perlin_octaves(
                    noise_positiver2 + (gposition.x / ((float) max_chunk_length)),
                    noise_positiver2 + (gposition.y / ((float) max_chunk_length)),
                    veggie_frequency,
                    seed, veggie_octaves);
                vmap->value[index] = choose_place(places, places_count, perlin_value);
                // zox_log("value veggie: %f", value);
            }
        }
        generate->value = zox_generate_tunk_mountains;
    }
} zox_sys_end(VegetationMapSystem);

/*byte value;
if (perlin_value >= flower_chance) {
    value = 4;  // Flowers
} else if (perlin_value >= tree_chance) {
    value = 3;  // Trees
} else if (perlin_value >= weeds_chance) {
    value = 2;  // Weeds
} else if (perlin_value >= grass_chance) {
    value = 1;  // Grass
} else {
    value = 0;  // dirt
}
vmap->value[index] = value;*/