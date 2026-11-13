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
        const entity terrain = zox_get_parent(world, e);
        TerrainHeights th;
        if (!terrain_heights_begin(world, terrain, &th)) {
            zox_loge("Invalid terrain, realm or its biomes");
            continue;
        }
        const byte terrain_depth = zox_getv(terrain, NodeDepth);
        const byte terrain_length = th.terrain_length;
        // now generate heightmap
        byte length = octree_size(lod->value);
        int2 map_size = int2_single(length);
        byte depth_difference = octree_size(terrain_depth - lod->value);
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
        int2 global_position = global_position_start;
        int2 position = int2_zero;
        resize_HeightMap(heightmap, length * length);
        for (position.x = 0; position.x < length; position.x++, global_position.x += depth_difference) {
            global_position.y = global_position_start.y;
            for (position.y = 0; position.y < length; position.y++, global_position.y += depth_difference) {
                const int index = int2_array_index(position, map_size);
                heightmap->value[index] = terrain_height_at(&th, global_position);
            }
        }
        generate->value = zox_generate_tunk_vegetation;
    }
    zox_sys_on_end();
} zoxd_system(height_map_system);
