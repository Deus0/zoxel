zox_sys2(HeightMapSystem) {
    // TODO: use height frequency from biome maps
    byte terrain_height_multiplier = 3;
    int terrain_octaves = 4; // 8
    // double height_frequency = 0.1; // terrain_frequency * 10;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkLod);
    zox_sys_in(TunkPosition);
    zox_sys_in(BiomeMap);
    zox_sys_out(GenerateTunk);
    zox_sys_out(HeightMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkLod, lod);
        zox_sys_i(TunkPosition, tunk_position);
        zox_sys_i(BiomeMap, biome_map);
        zox_sys_o(GenerateTunk, generate);
        zox_sys_o(HeightMap, heightmap);
        if (generate->value != zox_generate_tunk_heights) {
            continue;
        }
#ifdef zox_safety_checks
        if (!zox_disable_biomes && (!biome_map->length || !biome_map->value)) {
            zox_logw("[biome_map] biome map wasn't generated in time");
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
        // now generate heightmap
        byte length = octree_size(lod->value);
        int2 map_size = int2_single(length);
        byte depth_difference = octree_size(terrain_depth - lod->value);
        byte max_height = int_min(render_distance_y * (terrain_length - 1), 255);
#ifdef zox_safety_checks
        if (!zox_disable_biomes && biome_map->length != length * length) {
            zox_logw("[biome_map] invalid size");
            continue;
        }
#endif
        int2 global_position_start = (int2) {
            tunk_position->value.x * terrain_length,
            tunk_position->value.y * terrain_length
        };
        entity biome = 0;
        float frequency = 0;
        int2 global_position = global_position_start;
        int2 position = int2_zero;
        resize_HeightMap(heightmap, length * length);
        for (position.x = 0; position.x < length; position.x++, global_position.x += depth_difference) {
            global_position.y = global_position_start.y;
            for (position.y = 0; position.y < length; position.y++, global_position.y += depth_difference) {
                int index = int2_array_index(position, map_size);
                if (zox_flatlands) {
                    heightmap->value[index] = max_height / 2;
                    continue;
                }
                byte biome_id = !zox_disable_biomes ?
                    biome_map->value[index] :
                    0;
#ifdef zox_safety_checks
                if (biome_id >= realm_biomes->length) {
                    zox_loge("Biome ID OOB [%i] of [%i]", biome_id, realm_biomes->length);
                    continue;
                }
#endif
                // TODO: Blend frequency amongst several nearby ones
                // double frequency = biome_id == 0 ? height_frequency : height_frequency * 2;
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
                    frequency = zox_getv(biome, BiomeHeightFrequency);
                }
                //  double frequency = height_frequency;
                double perlin_x = noise_positiver2 + (global_position.x / ((float) terrain_length));
                double perlin_y = noise_positiver2 + (global_position.y / ((float) terrain_length));
                double perlin_value = perlin_octaves(perlin_x, perlin_y, frequency, seed, terrain_octaves);
                int value = int_floorf(perlin_value * terrain_length * terrain_height_multiplier);
                heightmap->value[index] = int_clamp(value, 0, max_height);
            }
        }
        generate->value = zox_generate_tunk_vegetation;
    }
} zox_sys_end(HeightMapSystem);
