int sand_height = 6;
int grass_height = 8;
int stone_height = 18;

// NOTE: Fills land with Soils based on biomes
zox_sys2(LandfillChunk3System) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(TunkLink);
    zox_sys_in(ChunkPosition);
    zox_sys_in(NodeDepth);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, state);
        zox_sys_i(TunkLink, tunk);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_i(NodeDepth, depth);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(VoxelNodeDirty, dirty);
        if (state->value != zox_dirty_active) {
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
        if (!zox_valid(tunk->value)) {
            zox_log_error("Invalid [Tunk] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        zox_geter(tunk->value, BiomeMap, biome_map);
        zox_geter(tunk->value, HeightMap, height_map);
        if (!height_map->length) {
            zox_log_error("Invalid [Tunk] [height_map] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        byte voctree_length = powers_of_two_byte[depth->value];
        int3 chunk_block_position = chunk_position_to_block_position(cposition->value, terrain_depth); // depth->value);
        byte terrain_chunk_length = powers_of_two_byte[terrain_depth];
        int2 map_size = int2_single(terrain_chunk_length);
        // NOTE: Shouldnt this use terrain depth?? Tests failed
        byte is_bottom_chunk = cposition->value.y == -render_distance_y;
        // int chunk_voxel_position_y = cposition->value.y *  terrain_chunk_length;
        byte3 positionl;
        byte hmultiplier = powers_of_two[terrain_depth - depth->value];
        byte stone_dig = 4 / hmultiplier;
        // Debug only at center point
        if (dbg_log && !cposition->value.x && !cposition->value.z) {
            zox_log("Filling Land [%ix%ix%i] -> Depth [%i]", cposition->value.x, cposition->value.y, cposition->value.z, depth->value);
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
                int terrain_top_position = (int) height;
                int top_position = terrain_top_position - chunk_block_position.y;
                top_position /= hmultiplier;
                if (top_position < 0) {
                    // NOTE: This clears above it, sometimes chunks above it keep solids when increasing depths
                    // TODO: Think of a better way here
                    positionl.y = 0;
                    // set_clean_VoxelNode(voctree, depth->value, positionl, 0);
                    for (positionl.y = 0; positionl.y < voctree_length; positionl.y++) {
                        set_clean_VoxelNode(voctree, depth->value, positionl, 0);
                    }
                    continue;
                }
                top_position = int_clamp(top_position, 0, voctree_length - 1);
                if (dbg_log && !cposition->value.x && !cposition->value.z && !positionl.x && !positionl.z) {
                    zox_log("   + Top Land C [%i], TY [%i], LY [%i]", cposition->value.y, terrain_top_position, top_position);
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
                    // if (terrain_position_y <= 1) {
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
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(LandfillChunk3System);
