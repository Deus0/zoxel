// TODO: use terrains seed
// TODO: use height frequency from biome maps
// TODO: Check Sand Height in this system

void vegetation_map_system(iter* it) {
    zox_sys_on_begin();
    // uint seed = global_seed;
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
        if (!zox_disable_biomes && !biome_map->length) {
            zox_logw("[vegetation_map] biome_map map wasn't generated in time");
            continue;
        }
#endif
        entity terrain = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("Invalid terrain");
            continue;
        }
#endif
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
        lint seed = zox_getv(terrain, Seed);
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        byte terrain_length = octree_size(terrain_depth);
        byte depth_difference = octree_size(terrain_depth - lod->value);
        // now generate heights
        byte length = octree_size(lod->value);
        int2 map_size = int2_single(length);
        // the biome's ground cover shares and tree chance (vegetation/fun)
        float shares[4] = { 0 };
        float tree_chance = 0;
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
                byte biome_id = !zox_disable_biomes ?
                    biome_map->value[index] :
                    0;
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
                    shares[0] = zox_getv(biome, DirtChance);
                    shares[1] = zox_getv(biome, GrassChance);
                    shares[2] = zox_getv(biome, WeedsChance);
                    shares[3] = zox_getv(biome, FlowerChance);
                    tree_chance = zox_getv(biome, TreeChance);
                }
                vegetation_map->value[index] = is_tree_at(global_position, seed, tree_chance) ?
                    zox_vegetation_trees :
                    get_ground_cover(global_position, terrain_length, seed, shares);
            }
        }
        generate->value = zox_generate_tunk_mountains;
    }
    zox_sys_on_end();
} zoxd_system(vegetation_map_system);

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
