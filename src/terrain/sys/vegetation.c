// NOTE: Places vegetation, only in top chunks
zox_sys2(VegetationChunk3System) {
    byte max_process = 16;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkPosition);
    zox_sys_in(TunkLink);
    zox_sys_out(GenerateChunk);
    zox_sys_out(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkPosition, chunk_position);
        zox_sys_i(TunkLink, tunk);
        zox_sys_o(GenerateChunk, generate);
        zox_sys_o(VoxelNode, voctree);
        if (generate->value != zox_generate_tchunk_vegetation) {
            continue;
        }
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
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
        zox_geter_value(terrain, NodeDepth, byte, terrain_depth);
        byte is_max_depth = depth->value == terrain_depth;
        byte voctree_length = powers_of_two_byte[depth->value];
        int terrain_chunk_length = powers_of_two[terrain_depth];
        int3 chunk_block_position = chunk_position_to_block_position(chunk_position->value, terrain_depth); // depth->value);
        int2 map_size = int2_single(terrain_chunk_length);
        byte3 positionl;
        byte hmultiplier = powers_of_two[terrain_depth - depth->value];
        if (!zox_valid(tunk->value)) {
            zox_log_error("[Vegetation] Invalid [Tunk] at [%ix%ix%i]", chunk_position->value.x, chunk_position->value.y, chunk_position->value.z);
            continue;
        }
        zox_geter(tunk->value, HeightMap, height_map);
        zox_geter(tunk->value, BiomeMap, biome_map);
        zox_geter(tunk->value, VegetationMap, vegetation_map);
        if (!vegetation_map->length) {
            zox_log_error("Invalid [Tunk] [vegetation_map] at [%ix%ix%i]", chunk_position->value.x, chunk_position->value.y, chunk_position->value.z);
            continue;
        }
        entity biome = 0;
        byte soil_grass_id = 0;
        byte grass_id = 0;
        byte wood_id = 0;
        write_lock_VoxelNode(voctree);
        for (positionl.x = 0; positionl.x < voctree_length; positionl.x++) {
            for (positionl.z = 0; positionl.z < voctree_length; positionl.z++) {
                int2 map_position = (int2) { positionl.x * hmultiplier, positionl.z * hmultiplier };
                int map_index = int2_array_index(map_position, map_size);
                byte biome_id = biome_map->value[map_index];
                byte veggie = vegetation_map->value[map_index];
                byte height = height_map->value[map_index];
                // NOTE: No need for vegetation under the sea
                //  (maybe some sea weed later)
                if (height <= grass_height || height >= stone_height) {
                    continue;
                }
                // Get Top Positions from Height Map
                /* int top_position = height - chunk_block_position.y;
                top_position /= hmultiplier;
                if (top_position < 0 || top_position >= voctree_length) {
                    continue;
                }*/
                // NOTE: Checks if outer bounds to determine if on top of world
                if (biome_id >= realm_biomes->length) {
                    zox_loge("Biome ID OOB [%i] of [%i]", biome_id, realm_biomes->length);
                    continue;
                }
                entity target_biome = realm_biomes->value[biome_id];
                if (!zox_valid(target_biome)) {
                    zox_loge("Biome is invalid [%i]", biome_id);
                    continue;
                }
                // NOTE: Updates our cache of our biome
                if (biome != target_biome) {
                    biome = target_biome;
                    // Get our biome vegetation blocks
                    entity soil_grass = zox_get_child_by_id(world, biome, zox_id(BlockSoilGrass));
                    entity grass = zox_get_child_by_id(world, biome, zox_id(BlockGrass));
                    entity wood = zox_get_child_by_id(world, biome, zox_id(BlockWood));
                    // Get IDs from Entitites
                    soil_grass_id = zox_valid(soil_grass) ? zox_getv(soil_grass, BlockIndex) : 0;
                    grass_id = zox_valid(grass) ? zox_getv(grass, BlockIndex) : 0;
                    wood_id = zox_valid(wood) ? zox_getv(wood, BlockIndex) : 0;
                }
                if (veggie >= 1) {
                    // NOTE: Make dirt Soil Grass
                    int global_y = height;
                    if (global_y >= chunk_block_position.y) {
                        positionl.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (positionl.y >= 0 && positionl.y < voctree_length) {
                            if (height >= chunk_block_position.y && height < chunk_block_position.y + voctree_length) {
                                set_VoxelNode(voctree, depth->value, positionl, soil_grass_id, 0);
                            }
                        }
                    }
                }
                // NOTE: Only do other vegetation if max depth
                if (!is_max_depth) {
                    continue;
                }
                if (veggie == 2) {
                    // Place Grass on tops
                    if (grass_id) {
                        int global_y = height + 1;
                        if (global_y >= chunk_block_position.y) {
                            positionl.y = (global_y - chunk_block_position.y) / hmultiplier;
                            if (positionl.y >= 0 && positionl.y < voctree_length) {
                                if (!getv_VoxelNode(voctree, positionl, depth->value)) {
                                    set_VoxelNode(voctree, depth->value, positionl, grass_id, 0);
                                }
                            }
                        }
                    }
                } else if (veggie == 3) {
                    // TODO: Grow through chunks neighbors
                    // Trees
                    for (int h = 1; h <= 2 + rand() % 4; h++) {
                        int global_y = height + h;
                        if (global_y < chunk_block_position.y) {
                            break;
                        }
                        positionl.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (positionl.y >= 0 && positionl.y < voctree_length) {
                            set_VoxelNode(voctree, depth->value, positionl, wood_id, 0);
                        }
                    }
                }
            }
        }
        write_unlock_VoxelNode(voctree);
        generate->value = zox_generate_tchunk_towns;
        zox_sys_increment();
    }
} zox_sys_end(VegetationChunk3System);
