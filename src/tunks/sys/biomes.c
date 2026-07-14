// Generates a Biome Map
zox_sys2(BiomeMapSystem) {
    const uint seed = global_seed;
    double biome_frequency = 0.04;
    byte biome_octaves = 2;
    zox_sys_begin();
    zox_sys_in(TunkLod);
    zox_sys_in(TunkPosition);
    zox_sys_out(GenerateTunk);
    zox_sys_out(BiomeMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TunkLod, lod);
        zox_sys_i(TunkPosition, tunk_position);
        zox_sys_o(GenerateTunk, generate);
        zox_sys_o(BiomeMap, biomes);
        if (generate->value != zox_generate_tunk_biomes) {
            continue;
        }
        // now generate heights
        byte terrain_length = octree_size(terrain_depth);
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
                double perlin_value = perlin_octaves(
                    noise_positiver2 + (global_position.x / ((float) terrain_length)),
                    noise_positiver2 + (global_position.y / ((float) terrain_length)),
                    biome_frequency,
                    seed,
                    biome_octaves
                );
                byte value = perlin_value > 0.5 ? 1 : 0;
                biomes->value[index] = value;
                // zox_log("biome value [%i]", value);
            }
        }
        generate->value = zox_generate_tunk_heights;
    }
} zox_sys_end(BiomeMapSystem);
