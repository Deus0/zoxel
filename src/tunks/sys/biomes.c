// Generates a Biome Map
zox_sys2(BiomeMapSystem) {
    byte dbg_log = 0;
    double biome_frequency = 0.02;
    byte biome_octaves = 12;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkLod);
    zox_sys_in(TunkPosition);
    zox_sys_out(GenerateTunk);
    zox_sys_out(BiomeMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkLod, lod);
        zox_sys_i(TunkPosition, tunk_position);
        zox_sys_o(GenerateTunk, generate);
        zox_sys_o(BiomeMap, biomes);
        if (generate->value != zox_generate_tunk_biomes) {
            continue;
        }
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
        byte biomes_count = byte_clamp(realm_biomes->length, 0, 255);
        lint seed = zox_getv(terrain, Seed);
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        byte terrain_length = octree_size(terrain_depth);
        // now generate heights
        byte length = octree_size(lod->value);
        int2 map_size = int2_single(length);
        byte depth_difference = octree_size(terrain_depth - lod->value);
        int2 global_position_start = (int2) {
            tunk_position->value.x * terrain_length,
            tunk_position->value.y * terrain_length
        };
        int2 global_position = global_position_start;
        int2 position = int2_zero;
        resize_BiomeMap(biomes, length * length);
        for (position.x = 0; position.x < length; position.x++, global_position.x += depth_difference) {
            global_position.y = global_position_start.y;
            for (position.y = 0; position.y < length; position.y++, global_position.y += depth_difference) {
                int index = int2_array_index(position, map_size);
                //  double frequency = height_frequency;
                double perlin_x = noise_positiver2 + (global_position.x / ((float) terrain_length));
                double perlin_y = noise_positiver2 + (global_position.y / ((float) terrain_length));
                double perlin_value = perlin_octaves(perlin_x, perlin_y, biome_frequency, seed, biome_octaves);
                // byte value = perlin_value > 0.5 ? 1 : 0;
                // zox_log("biome value [%i]", value);
                byte biome_index = (byte)(perlin_value * biomes_count);
                if (biome_index >= biomes_count) {
                    biome_index = biomes_count - 1;
                }
                biomes->value[index] = biome_index;
                if (dbg_log >= 2 && biome_index == biomes_count - 1) {
                    zox_log("Biome [%ix%i]: %i", position.x, position.y, biome_index);
                }
            }
        }
        generate->value = zox_generate_tunk_heights;
    }
} zox_sys_end(BiomeMapSystem);
