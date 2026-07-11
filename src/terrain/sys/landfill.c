short sand_height = 2;
short grass_height = 4;
short stone_height = 18;

// NOTE: Fills land with Soils based on biomes
zox_sys2(LandfillChunk3System) {
    byte max_process = 16;
    byte dbg_log = 0;
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
        if (generate->value != zox_generate_tchunk_landfill) {
            continue;
        }
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            zox_loge("No Terrain on Chunk");
            continue;
        }
        entity realm = zox_get_parent(world, terrain);
        if (!zox_valid(realm)) {
            zox_loge("No Realm on Terrain");
            continue;
        }
        zox_geter(realm, BiomeLinks, realm_biomes);
        if (!realm_biomes->length) {
            zox_loge("No Biomes on Realm");
            continue;
        }
        if (!zox_valid(tunk->value)) {
            zox_loge("[Landfill] ]nvalid [Tunk] at [%ix%ix%i]", chunk_position->value.x, chunk_position->value.y, chunk_position->value.z);
            continue;
        }
        zox_geter(tunk->value, BiomeMap, biome_map);
        if (!biome_map->length) {
            zox_loge("Invalid [Tunk] [biome_map] at [%ix%ix%i]", chunk_position->value.x, chunk_position->value.y, chunk_position->value.z);
            continue;
        }
        zox_geter(tunk->value, HeightMap, height_map);
        if (!height_map->length) {
            zox_loge("Invalid [Tunk] [height_map] at [%ix%ix%i]", chunk_position->value.x, chunk_position->value.y, chunk_position->value.z);
            continue;
        }
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        byte voctree_length = powers_of_two_byte[depth->value];
        int3 chunk_block_position = chunk_position_to_block_position(chunk_position->value, terrain_depth);
        byte terrain_chunk_length = powers_of_two_byte[terrain_depth];
        int2 map_size = int2_single(terrain_chunk_length);
        // NOTE: Shouldnt this use terrain depth?? Tests failed
        byte is_bottom_chunk = chunk_position->value.y == -render_distance_y;
        // int chunk_voxel_position_y = chunk_position->value.y *  terrain_chunk_length;
        byte3 positionl;
        byte hmultiplier = powers_of_two[terrain_depth - depth->value];
        byte stone_dig = 4 / hmultiplier;
        // Debug only at center point
        if (dbg_log && !chunk_position->value.x && !chunk_position->value.z) {
            zox_log("Filling Land [%ix%ix%i] -> Depth [%i]", chunk_position->value.x, chunk_position->value.y, chunk_position->value.z, depth->value);
        }
        // Get realm blocks first
        entity obsidian = zox_get_child_by_id(world, realm, zox_id(BlockObsidian));
        byte obsidian_id = zox_valid(obsidian) ? zox_getv(obsidian, BlockIndex) : 0;
        if (!obsidian_id) {
            zox_logw("Realm [%s] Has no Obsidian Block.", zox_get_name(realm));
        }
        // set our variables for our loop
        entity biome = 0;
        byte soil_id = 0;
        byte sand_id = 0;
        byte stone_id = 0;
        write_lock_VoxelNode(voctree);
        for (positionl.x = 0; positionl.x < voctree_length; positionl.x++) {
            for (positionl.z = 0; positionl.z < voctree_length; positionl.z++) {
                // NOTE: This converts a 2D map to a chunk value by scaling
                int2 map_position = (int2) { positionl.x * hmultiplier, positionl.z * hmultiplier };
                int map_index = int2_array_index(map_position, map_size);
                byte biome_id = biome_map->value[map_index];
                byte height = height_map->value[map_index];
                // Get Top Positions from Height Map
                if (height < chunk_block_position.y) {
                    // NOTE: This clears above it, sometimes chunks above it keep solids when increasing depths
                    // TODO: Think of a better way here
                    /*positionl.y = 0;
                    for (positionl.y = 0; positionl.y < voctree_length; positionl.y++) {
                        set_clean_VoxelNode(voctree, depth->value, positionl, 0);
                    }*/
                    continue;
                }
                int terrain_top_position = height;
                int top_position = terrain_top_position - chunk_block_position.y;
                top_position /= hmultiplier;
                top_position = int_clamp(top_position, 0, voctree_length - 1);
                if (dbg_log && !chunk_position->value.x && !chunk_position->value.z && !positionl.x && !positionl.z) {
                    zox_log("   + Top Land C [%i], TY [%i], LY [%i]", chunk_position->value.y, terrain_top_position, top_position);
                }
                if (biome_id >= realm_biomes->length) {
                    zox_loge("Biome ID OOB [%i] of [%i]", biome_id, realm_biomes->length);
                    continue;
                }
                entity new_biome = realm_biomes->value[biome_id];
                if (!zox_valid(new_biome)) {
                    zox_loge("Biome is invalid [%i]", biome_id);
                    continue;
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
                // We fill the ground up here
                for (positionl.y = 0; positionl.y <= top_position; positionl.y++) {
                    int terrain_position_y = chunk_block_position.y + positionl.y * hmultiplier;
                    // top blocks
                    byte value;
                    if (is_bottom_chunk && positionl.y == 0) {
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
                    set_clean_VoxelNode(voctree, depth->value, positionl, value);
                }
            }
        }
        write_unlock_VoxelNode(voctree);
        // Completed
        generate->value = zox_generate_tchunk_vegetation;
        zox_sys_increment();
    }
} zox_sys_end(LandfillChunk3System);
