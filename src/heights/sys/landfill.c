// TODO: Multiply by the world height
short terrain_sand_height = 6; // 16;
short terrain_grass_height = 7; // 18;
short terrain_stone_height = 18; // 32;

// TODO: Refactor heightmaps into quadtrees.
// TODO: Build terrain with recursive region fill instead of per-voxel writes.
//       Traverse the quadtree and octree together, subdividing only where
//       terrain or material boundaries require it.

// NOTE: Fills land with Soils based on biomes
zox_sys2(LandfillChunkSystem) {
    byte dbg_log = 0;
    byte disable_biome_changes = 1;
    uint pcount = 0;
    byte max_process = !zox_disable_process_skips ? terrain_depth : 0;
    short sand_height = terrain_sand_height * render_distance_y;
    short stone_height = terrain_stone_height * render_distance_y;
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
        if (max_process && pcount > max_process) {
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
        if (!zox_disable_biomes && !biome_map->length) {
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
        byte shift = terrain_depth - depth->value;
        short length = octree_size(depth->value);
        // byte hmultiplier = octree_size(shift);
        int3 chunk_block_position = chunk_position_to_block_position(chunk_position->value, terrain_depth);
        int2 map_size = int2_single(length);
        // NOTE: Shouldnt this use terrain depth?? Tests failed
        byte is_bottom_chunk = chunk_position->value.y == -render_distance_y;
        byte3 position;
        // We using lod depths now
        byte stone_dig = 4 >> shift;
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
        // write_lock_VoxelNode(voctree);
        for (position.z = 0; position.z < length; position.z++) {
            int row = position.z * length;
            for (position.x = 0; position.x < length; position.x++) {
                int map_index = row + position.x;
                // NOTE: This converts a 2D map to a chunk value by scaling
                // int2 map_position = (int2) { position.x, position.z };
                // int map_index = int2_array_index(map_position, map_size);
#ifdef zox_safety_checks
                if (!zox_disable_biomes && map_index >= biome_map->length) {
                    zox_loge("Landfill: Map Index OOB [%i] : [%i].. Pos [%ix%i] Size [%ix%i]", map_index, biome_map->length, position.x, position.y, map_size.x, map_size.y);
                    continue;
                }
#endif
                byte height = height_map->value[map_index];
                // Ignore fill if too high
                if (height < chunk_block_position.y) {
                    continue;
                }
                byte biome_id = !zox_disable_biomes ?
                    biome_map->value[map_index] :
                    0;
                // Get Top Positions from Height Map
                byte local_height = (height - chunk_block_position.y) >> shift;
                local_height = int_clamp(local_height, 0, length - 1);
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
                // sticks to one biome per chunk
                if (disable_biome_changes && biome) {
                    new_biome = biome;
                }
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
                byte top_material;
                if (height >= stone_height) {
                    top_material = stone_id;
                } else if (height >= sand_height) {
                    top_material = soil_id;
                } else {
                    top_material = sand_id;
                }
                int stone_end = (height - stone_dig - chunk_block_position.y) >> shift;
                stone_end = int_clamp(stone_end, -1, local_height);
                position.y = 0;
                // Bottom obsidian layer.
                if (is_bottom_chunk) {
                    set_clean_VoxelNode(
                        voctree,
                        depth->value,
                        position,
                        obsidian_id ? obsidian_id : soil_id
                    );
                    position.y = 1;
                }
                // Fill all stone.
                for (; position.y <= stone_end; position.y++) {
                    set_clean_VoxelNode(
                        voctree,
                        depth->value,
                        position,
                        stone_id
                    );
                }
                // Fill top material.
                for (; position.y <= local_height; position.y++) {
                    set_clean_VoxelNode(
                        voctree,
                        depth->value,
                        position,
                        top_material
                    );
                }
                // We fill the ground up here
                /*int terrain_position_y = chunk_block_position.y;
                for (position.y = 0; position.y <= local_height; position.y++) {
                    terrain_position_y += hmultiplier;
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
                }*/
                if (dbg_log) {
                    zox_log("[%s]:Landfill [%ix%i] -> H [%i] : GH [%i] L [%i]", zox_getn(e), position.x, position.z, local_height, height, length);
                }
            }
        }
        // write_unlock_VoxelNode(voctree);
        // Completed
        generate->value = zox_generate_terrain_vegetation;
        pcount += depth->value;
    }
} zox_sys_end(LandfillChunkSystem);
