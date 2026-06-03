// NOTE: Fills land with Soils based on biomes
zox_sys2(LandfillChunk3System) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkPosition);
    zox_sys_in(VoxLink);
    zox_sys_in(TunkLink);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, state);
        zox_sys_i(NodeDepth, voctree_depth);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_i(VoxLink, terrain);
        zox_sys_i(TunkLink, tunk);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(VoxelNodeDirty, dirty);
        if (state->value != zox_dirty_active) {
            continue;
        }
        zox_geter_value(terrain->value, RealmLink, entity, realm);
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
        zox_geter(tunk->value, BiomeMap, biome_map);
        zox_geter(tunk->value, HeightMap, height_map);
        if (!height_map->length) {
            zox_log_error("Invalid [Tunk] [Maps] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        byte voctree_length = powers_of_two_byte[voctree_depth->value];
        zox_geter_value(terrain->value, NodeDepth, byte, terrain_depth);
        // byte terrain_chunk_length = voctree_length;
        byte terrain_chunk_length = powers_of_two_byte[terrain_depth];
        // NOTE: Shouldnt this use terrain depth?? Tests failed
        byte is_bottom_chunk = cposition->value.y == -render_distance_y;
        int chunk_position_y = cposition->value.y * terrain_chunk_length;
        byte3 positionl;
        int hmultiplier = 1;
        byte ccc = voctree_depth->value;
        while (ccc != terrain_depth) {
            hmultiplier *= 2;
            ccc++;
        }
        int2 hsize = int2_single(terrain_chunk_length);
        // Debug only at center point
        if (dbg_log && !cposition->value.x && !cposition->value.z) {
            zox_log("Filling Land [%ix%ix%i] -> Depth [%i]", cposition->value.x, cposition->value.y, cposition->value.z, voctree_depth->value);
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
                int map_index = int2_array_index(map_position, hsize);
                byte biome_id = biome_map->value[map_index];
                byte height = height_map->value[map_index];
                int terrain_top_position = (int) height;
                int top_position = terrain_top_position - chunk_position_y;
                top_position /= hmultiplier;
                if (top_position < 0) {
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
                entity target_biome = realm_biomes->value[biome_id];
                if (!zox_valid(target_biome)) {
                    zox_loge("Biome is invalid [%i]", biome_id);
                    continue;
                }
                // NOTE: Updates our cache of our biome
                if (biome != target_biome) {
                    biome = target_biome;
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
                    int terrain_position_y = chunk_position_y + positionl.y;
                    // top blocks
                    byte value;
                    // If Bottom Position
                    if (is_bottom_chunk && positionl.y == 0) {
                        value = obsidian_id > 0 ? obsidian_id : soil_id;
                    } else if (terrain_position_y >= terrain_top_position - 2) {
                        value = terrain_position_y > sand_height ? soil_id : sand_id;
                    } else {
                        value = stone_id;
                    }
                    set_clean_VoxelNode(voctree, voctree_depth->value, positionl, value);
                }
            }
        }
        write_unlock_VoxelNode(voctree);
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(LandfillChunk3System);
