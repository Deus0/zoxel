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
    zox_sys_in(TunkLod);
    zox_sys_in(TunkPosition);
    zox_sys_in(BiomeMap);
    zox_sys_out(GenerateTunk);
    zox_sys_out(VegetationMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkLod, lod);
        zox_sys_i(TunkPosition, tunk_position);
        zox_sys_i(BiomeMap, biome_map);
        zox_sys_o(GenerateTunk, generate);
        zox_sys_o(VegetationMap, vegetation_map);
        if (generate->value != zox_generate_tunk_vegetation) {
            continue;
        }
        if (zox_disable_vegetation) {
            generate->value = zox_generate_tunk_mountains;
            continue;
        }
#ifdef zox_safety_checks
        if (!biome_map->length) {
            zox_logw("[vegetation_map] biome_map map wasn't generated in time");
            continue;
        }
#endif
        entity terrain = zox_get_parent(world, e);
        entity realm = zox_get_parent(world, terrain);
#ifdef zox_safety_checks
        if (!zox_valid(realm)) {
            zox_loge("Invalid realm");
            continue;
        }
#endif
        zox_geter(realm, BiomeLinks, realm_biomes);
#ifdef zox_safety_checks
        if (!realm_biomes->length) {
            zox_loge("No Biomes on Realm");
            continue;
        }
#endif
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        byte terrain_length = octree_size(terrain_depth);
        byte depth_difference = octree_size(terrain_depth - lod->value);
        // now generate heights
        byte length = octree_size(lod->value);
        int2 map_size = int2_single(length);
        byte places_count = 5;
        place_chance places[places_count];
        memset(places, 0, sizeof(places));
        places[0].value = zox_vegetation_dirt;
        places[1].value = zox_vegetation_grass;
        places[2].value = zox_vegetation_weeds;
        places[3].value = zox_vegetation_trees;
        places[4].value = zox_vegetation_flowers;
        int2 global_position_start = (int2) {
            tunk_position->value.x * terrain_length,
            tunk_position->value.y * terrain_length
        };
        int2 global_position = global_position_start;
        entity biome = 0;
        int2 position = int2_zero;
        resize_VegetationMap(vegetation_map, length * length);
        for (position.x = 0; position.x < length; position.x++, global_position.x += depth_difference) {
            global_position.y = global_position_start.y;
            for (position.y = 0; position.y < length; position.y++, global_position.y += depth_difference) {
                int index = int2_array_index(position, map_size);
                // Get Biome Data
                byte biome_id = biome_map->value[index];
#ifdef zox_safety_checks
                if (biome_id >= realm_biomes->length) {
                    zox_loge("Biome ID OOB [%i] of [%i]", biome_id, realm_biomes->length);
                    continue;
                }
#endif
                entity new_biome = realm_biomes->value[biome_id];
#ifdef zox_safety_checks
                if (!zox_valid(new_biome)) {
                    zox_loge("Biome is invalid [%i]", biome_id);
                    continue;
                }
#endif
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
                    noise_positiver2 + (global_position.x / ((float) terrain_length)),
                    noise_positiver2 + (global_position.y / ((float) terrain_length)),
                    veggie_frequency,
                    seed, veggie_octaves);
                vegetation_map->value[index] = choose_place(places, places_count, perlin_value);
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
vegetation_map->value[index] = value;*/