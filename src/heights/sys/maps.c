void height_map_system(iter* it) {
    zox_sys_on_begin();
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
        // each biome's height frequency, heights blend between them near their edges (biomes/fun/blend.c)
        float frequencies[zox_biomes_max];
        const byte biomes_count = zox_disable_biomes ? 1 : (byte) int_min(realm_biomes->length, zox_biomes_max);
        for (byte b = 0; b < biomes_count; b++) {
            const entity biome = realm_biomes->value[b];
            frequencies[b] = zox_valid(biome) ? zox_getv(biome, BiomeHeightFrequency) : 0.01f;
        }
        const BiomeClimates climates = get_biome_climates(terrain_length, seed, biomes_count);
        int2 global_position = global_position_start;
        int2 position = int2_zero;
        resize_HeightMap(heightmap, length * length);
        for (position.x = 0; position.x < length; position.x++, global_position.x += depth_difference) {
            global_position.y = global_position_start.y;
            for (position.y = 0; position.y < length; position.y++, global_position.y += depth_difference) {
                const int index = int2_array_index(position, map_size);
                if (zox_flatlands) {
                    heightmap->value[index] = max_height / 2;
                    continue;
                }
                heightmap->value[index] = get_terrain_height(global_position, terrain_length, seed,
                    frequencies, &climates, max_height, zox_biome_blend);
            }
        }
        generate->value = zox_generate_tunk_vegetation;
    }
    zox_sys_on_end();
} zoxd_system(height_map_system);
