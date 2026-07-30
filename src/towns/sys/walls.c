// NOTE: Uses Height Map + Town Map to spawn Town Walls in chunks
zox_sys2(TownWallsSystem) {
    // TODO: Link the Chunk to Town and use hte town data
    // TODO: get the Home from the Town based on position
    byte dbg_log = 0;
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
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_i(TunkLink, tunk);
        zox_sys_o(GenerateChunk, generate);
        zox_sys_o(VoxelNode, voctree);
        if (generate->value != zox_generate_terrain_towns) {
            continue;
        }
        if (zox_disable_towns) {
            generate->value = zox_generate_terrain_sunlight;
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        entity realm = zox_get_parent(world, terrain);
#ifdef zox_safety_checks
        if (!zox_valid(realm)) {
            continue;
        }
#endif
        zox_geter(realm, BiomeLinks, realm_biomes);
#ifdef zox_safety_checks
        if (!realm_biomes->length) {
            zox_log_error("No Biomes on Realm");
            continue;
        }
#endif
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        entity wall = zox_get_child_by_id(world, realm, zox_id(BlockBricks));
#ifdef zox_safety_checks
        if (!zox_valid(wall)) {
            zox_loge("No wall for town..");
            continue;
        }
#endif
        byte bricks_id = zox_getv(wall, BlockIndex);
#ifdef zox_safety_checks
        if (!bricks_id) {
            zox_loge("Bricks ID Invalid");
            continue;
        }
#endif
        // Home Placeholders
        byte home_wall_id = bricks_id;
        byte home_floor_id = bricks_id;
        byte home_roof_id = bricks_id;
        byte length = octree_size(depth->value);
        int3 chunk_block_position = chunk_position_to_block_position(cposition->value, terrain_depth);
        int2 map_size = int2_single(length); // max_chunk_length);
        byte hmultiplier = octree_size(terrain_depth - depth->value);
        // int chunk_position_y = chunk_block_position.y;
        byte gate_height = 6; // rand_range(4, 8);
        byte wall_height = 8; // rand_range(4, 8);
        byte home_height = 6; // rand_range(4, 8);
#ifdef zox_safety_checks
        if (!zox_valid(tunk->value)) {
            zox_loge("[Towns] Invalid [Tunk] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
#endif
        zox_geter(tunk->value, BiomeMap, biome_map);
        zox_geter(tunk->value, HeightMap, height_map);
        zox_geter(tunk->value, TownMap, town_map);
#ifdef zox_safety_checks
        if (!town_map->length) {
            zox_log_error("Invalid [Tunk] [town_map] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
#endif
        // write_lock_VoxelNode(voctree);
        entity biome = 0;
        byte road_id = 0;
        byte3 position;
        for (position.x = 0; position.x < length; position.x++) {
            for (position.z = 0; position.z < length; position.z++) {
                int2 map_position = (int2) { position.x, position.z };
                int map_index = int2_array_index(map_position, map_size);
#ifdef zox_safety_checks
                if (map_index >= town_map->length) {
                    zox_loge("Town: Map Index OOB [%i] : [%i].. Pos [%ix%i] Size [%ix%i]", map_index, town_map->length, map_position.x, map_position.y, map_size.x, map_size.y);
                    continue;
                }
#endif
                byte biome_id = biome_map->value[map_index];
                // NOTE: Checks if outer bounds to determine if on top of world
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
                    entity road = zox_get_child_by_id(world, biome, zox_id(BlockRoad));
                    road_id = zox_valid(road) ? zox_getv(road, BlockIndex) : 0;
                }
                byte height = height_map->value[map_index];
                byte town_value = town_map->value[map_index];
                if (!town_value) {
                    continue;
                }
                if (dbg_log) {
                    zox_log("Placing Town Wall at [%ix%i]", chunk_block_position.x + position.x, chunk_block_position.z + position.z);
                }
                // byte wall_height = zox_getv(town, Height);
                if (town_value == zox_town_type_zone) {
                    int global_y = height;
                    if (global_y >= chunk_block_position.y) {
                        position.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (position.y >= 0 && position.y < length) {
                            set_clean_VoxelNode(voctree, depth->value, position, road_id);
                        }
                    }
                } else if (town_value == zox_town_type_wall || town_value == zox_town_type_wall_edge || town_value == zox_town_type_wall_spike) {
                    byte is_spike = town_value == zox_town_type_wall_spike;
                    byte is_edge = town_value == zox_town_type_wall_edge;
                    for (byte h = 1; h <= wall_height + is_edge + is_spike * 2; h++) {
                        int global_y = height + h;
                        if (global_y >= chunk_block_position.y) {
                            position.y = (global_y - chunk_block_position.y) / hmultiplier;
                            if (position.y >= 0 && position.y < length) {
                                set_clean_VoxelNode(voctree, depth->value, position, bricks_id);
                            }
                        }
                    }
                }
                // NOTE: Town Gate! has a gap!
                else if (town_value == zox_town_type_gate) {
                    for (int h = gate_height; h <= wall_height; h++) {
                        int global_y = height + h;
                        if (global_y >= chunk_block_position.y) {
                            position.y = (global_y - chunk_block_position.y) / hmultiplier;
                            if (position.y >= 0 && position.y < length) {
                                set_clean_VoxelNode(voctree, depth->value, position, bricks_id);
                            }
                        }
                    }
                }
                // Home Walls
                else if (town_value == zox_town_type_home_wall) {
                    for (int h = 2; h < home_height; h++) {
                        int global_y = height + h;
                        if (global_y >= chunk_block_position.y) {
                            position.y = (global_y - chunk_block_position.y) / hmultiplier;
                            if (position.y >= 0 && position.y < length) {
                                set_clean_VoxelNode(voctree, depth->value, position, home_wall_id);
                            }
                        }
                    }
                }
                // Home Door Wall Overhang
                else if (town_value == zox_town_type_home_door) {
                    for (int h = home_height - 1; h < home_height; h++) {
                        int global_y = height + h;
                        if (global_y >= chunk_block_position.y) {
                            position.y = (global_y - chunk_block_position.y) / hmultiplier;
                            if (position.y >= 0 && position.y < length) {
                                set_clean_VoxelNode(voctree, depth->value, position, home_wall_id);
                            }
                        }
                    }
                }
                // Roofs
                if (town_value == zox_town_type_home || town_value == zox_town_type_home_door || town_value == zox_town_type_home_wall) {
                    int global_y = height + home_height;
                    if (global_y >= chunk_block_position.y) {
                        position.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (position.y >= 0 && position.y < length) {
                            set_clean_VoxelNode(voctree, depth->value, position, home_roof_id);
                        }
                    }
                }
                // Floors
                if (town_value == zox_town_type_home || town_value == zox_town_type_home_door || town_value == zox_town_type_home_wall) {
                    int global_y = height + 1;
                    if (global_y >= chunk_block_position.y) {
                        position.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (position.y >= 0 && position.y < length) {
                            set_clean_VoxelNode(voctree, depth->value, position, home_floor_id);
                        }
                    }
                }
            }
        }
        // write_unlock_VoxelNode(voctree);
        generate->value = zox_generate_terrain_sunlight;
    }
} zox_sys_end(TownWallsSystem);
