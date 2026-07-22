// TODO: Multiply by the world height
short sand_height = 12;
short grass_height = 16;
short stone_height = 28;

// NOTE: Fills land with Soils based on biomes
zox_sys2(LandfillChunkSystem) {
    byte dbg_log = 0;
    byte max_process = 0; // 8;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkLink);
    zox_sys_in(ChunkPosition);
    zox_sys_in(NodeDepth);
    zox_sys_out(GenerateChunk);
    zox_sys_out(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkLink, tunk);
        zox_sys_i(ChunkPosition, chunk_position);
        zox_sys_i(NodeDepth, depth);
        zox_sys_o(GenerateChunk, generate);
        zox_sys_o(VoxelNode, voctree);
        if (generate->value != zox_generate_terrain_landfill) {
            continue;
        }
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("No Terrain on Chunk");
            continue;
        }
#endif
        entity realm = zox_get_parent(world, terrain);
#ifdef zox_safety_checks
        if (!zox_valid(realm)) {
            zox_loge("No Realm on Terrain");
            continue;
        }
#endif
        zox_geter(realm, BiomeLinks, realm_biomes);
#ifdef zox_safety_checks
        if (!realm_biomes->length) {
            zox_loge("No Biomes on Realm");
            continue;
        }
        if (!zox_valid(tunk->value)) {
            zox_loge("[Landfill] ]nvalid [Tunk] at [%ix%ix%i]", chunk_position->value.x, chunk_position->value.y, chunk_position->value.z);
            continue;
        }
#endif
        // If still generating, we wait
        if (zox_getv(tunk->value, GenerateTunk)) {
            continue;
        }
        byte tunk_lod = zox_getv(tunk->value, TunkLod);
        if (depth->value != tunk_lod) {
            zox_loge("Chunk Depth is not Tunk Lod [%i] != [%i]", depth->value, tunk_lod);
            continue;
        }
        // zox_log("Chunk Depth IS Tunk Lod [%i] != [%i]", depth->value, tunk_lod);
        zox_geter(tunk->value, BiomeMap, biome_map);
#ifdef zox_safety_checks
        if (!biome_map->length) {
            zox_loge("Invalid [Tunk] [biome_map] at [%ix%ix%i]", chunk_position->value.x, chunk_position->value.y, chunk_position->value.z);
            continue;
        }
#endif
        zox_geter(tunk->value, HeightMap, height_map);
#ifdef zox_safety_checks
        if (!height_map->length) {
            zox_loge("Invalid [Tunk] [height_map] at [%ix%ix%i]", chunk_position->value.x, chunk_position->value.y, chunk_position->value.z);
            continue;
        }
#endif
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        short length = octree_size(depth->value);
        int3 chunk_block_position = chunk_position_to_block_position(chunk_position->value, terrain_depth);
        int2 map_size = int2_single(length);
        // NOTE: Shouldnt this use terrain depth?? Tests failed
        byte is_bottom_chunk = chunk_position->value.y == -render_distance_y;
        byte3 position;
        byte hmultiplier = powers_of_two[terrain_depth - depth->value];
        // We using lod depths now
        byte stone_dig = 4 / hmultiplier;
        // Get realm blocks first
        entity obsidian = zox_get_child_by_id(world, realm, zox_id(BlockObsidian));
        byte obsidian_id = zox_valid(obsidian) ? zox_getv(obsidian, BlockIndex) : 0;
#ifdef zox_safety_checks
        if (!obsidian_id) {
            zox_logw("Realm [%s] Has no Obsidian Block.", zox_get_name(realm));
        }
#endif
        // set our variables for our loop
        entity biome = 0;
        byte soil_id = 0;
        byte sand_id = 0;
        byte stone_id = 0;
        write_lock_VoxelNode(voctree);
        for (position.x = 0; position.x < length; position.x++) {
            for (position.z = 0; position.z < length; position.z++) {
                // NOTE: This converts a 2D map to a chunk value by scaling
                int2 map_position = (int2) { position.x, position.z };
                int map_index = int2_array_index(map_position, map_size);
#ifdef zox_safety_checks
                if (map_index >= biome_map->length) {
                    zox_loge("Landfill: Map Index OOB [%i] : [%i].. Pos [%ix%i] Size [%ix%i]", map_index, biome_map->length, map_position.x, map_position.y, map_size.x, map_size.y);
                    continue;
                }
#endif
                byte height = height_map->value[map_index];
                // Ignore fill if too high
                if (height < chunk_block_position.y) {
                    continue;
                }
                byte biome_id = biome_map->value[map_index];
                // Get Top Positions from Height Map
                byte local_height = int_clamp((height - chunk_block_position.y) / hmultiplier, 0, length - 1);
#ifdef zox_safety_checks
                if (biome_id >= realm_biomes->length) {
                    zox_loge("[Landfill] Biome ID OOB [%i] of [%i]", biome_id, realm_biomes->length);
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
                    // Get our biome vegetation blocks
                    entity soil = zox_get_child_by_id(world, biome, zox_id(BlockSoil));
                    entity sand = zox_get_child_by_id(world, biome, zox_id(BlockSand));
                    entity stone = zox_get_child_by_id(world, biome, zox_id(BlockStone));
                    // Get IDs from Entitites
                    soil_id = zox_valid(soil) ? zox_getv(soil, BlockIndex) : 0;
                    sand_id = zox_valid(sand) ? zox_getv(sand, BlockIndex) : 0;
                    stone_id = zox_valid(stone) ? zox_getv(stone, BlockIndex) : 0;
                }
                // We fill the ground up here
                for (position.y = 0; position.y <= local_height; position.y++) {
                    int terrain_position_y = chunk_block_position.y + position.y * hmultiplier;
                    // top blocks
                    byte value;
                    if (is_bottom_chunk && position.y == 0) {
                        value = obsidian_id > 0 ? obsidian_id : soil_id;
                    } else if (terrain_position_y <= height - stone_dig) {
                        value = stone_id;
                    } else {
                        // NOTE: Top blocks in fill map
                        if (height >= stone_height) {
                            value = stone_id;
                        } else if (height >= sand_height) {
                            value = soil_id;
                        } else {
                            value = sand_id;
                        }
                    }
                    set_clean_VoxelNode(voctree, depth->value, position, value);
                }
                if (dbg_log) {
                    zox_log("[%s]:Landfill [%ix%i] -> H [%i] : GH [%i] L [%i]", zox_getn(e), position.x, position.z, local_height, height, length);
                }
            }
        }
        write_unlock_VoxelNode(voctree);
        // Completed
        generate->value = zox_generate_terrain_vegetation;
        zox_sys_increment();
    }
} zox_sys_end(LandfillChunkSystem);
