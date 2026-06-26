// NOTE: Uses Height Map + Town Map to spawn Town Walls in chunks
zox_sys2(TownWallsSystem) {
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
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_i(VoxLink, terrain);
        zox_sys_i(TunkLink, tunk);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(VoxelNodeDirty, dirty);
        if (state->value != zox_dirty_active) {
            continue;
        }
        zox_geter_value(terrain->value, NodeDepth, byte, terrain_depth);
        entity realm = zox_getv(terrain->value, RealmLink);
        byte is_max_depth = depth->value == terrain_depth;
        /*if (!is_max_depth) {
            continue;
        }*/
        entity wall = zox_get_child_by_id(world, realm, zox_id(BlockBricks));
        if (!zox_valid(wall)) {
            zox_loge("No wall for town..");
            continue;
        }
        byte bricks_id = zox_getv(wall, BlockIndex);
        if (!bricks_id) {
            continue;
        }
        byte voctree_length = powers_of_two_byte[depth->value];
        // int3 chunk_block_position = chunk_position_to_block_position(cposition->value, depth->value);
        int3 chunk_block_position = chunk_position_to_block_position(cposition->value, terrain_depth); // voctree_depth->value);
        // int3 chunk_block_position = (int3) { cposition->value.x * voctree_length, cposition->value.y * voctree_length, cposition->value.z * voctree_length };
        int chunk_position_y = chunk_block_position.y; // cposition->value.y * voctree_length;
        byte3 positionl;
        byte wall_height = 6; // rand_range(4, 8);
        byte hmultiplier = powers_of_two[terrain_depth - depth->value];
        /*int hmultiplier = 1;
        byte ccc = depth->value;
        while (ccc != terrain_depth) {
            hmultiplier *= 2;
            // wall_height /= 2;
            ccc++;
        }*/
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 map_size = int2_single(max_chunk_length);
        if (!zox_valid(tunk->value)) {
            zox_log_error("Invalid [Tunk] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        zox_geter(tunk->value, HeightMap, height_map);
        zox_geter(tunk->value, TownMap, town_map);
        if (!town_map->length) {
            zox_log_error("Invalid [Tunk] [town_map] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        write_lock_VoxelNode(voctree);
        for (positionl.x = 0; positionl.x < voctree_length; positionl.x++) {
            for (positionl.z = 0; positionl.z < voctree_length; positionl.z++) {
                int2 map_position = (int2) { positionl.x * hmultiplier, positionl.z * hmultiplier };
                int map_index = int2_array_index(map_position, map_size);
                int height = (int) height_map->value[map_index];
                byte town_value = town_map->value[map_index];
                if (!town_value) {
                    continue;
                }
                if (dbg_log) {
                    zox_log("Placing Town Wall at [%ix%i]", chunk_block_position.x + positionl.x, chunk_block_position.z + positionl.z);
                }
                // byte wall_height = zox_getv(town, Height);
                if (town_value == 2) {
                    for (int h = 1; h <= wall_height; h++) {
                        int global_y = height + h;
                        positionl.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (positionl.y >= 0 && positionl.y < voctree_length) {
                            set_clean_VoxelNode(voctree, depth->value, positionl, bricks_id);
                        }
                    }
                }
                // NOTE: Town Gate! has a gap!
                else if (town_value == 3) {
                    for (int h = 4; h <= wall_height; h++) {
                        int global_y = height + h;
                        positionl.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (positionl.y >= 0 && positionl.y < voctree_length) {
                            set_clean_VoxelNode(voctree, depth->value, positionl, bricks_id);
                        }
                    }
                }
            }
        }
        write_unlock_VoxelNode(voctree);
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(TownWallsSystem);
