// NOTE: Uses Height Map + Town Map to spawn Town Walls in chunks
zox_sys2(TownWallsSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkPosition);
    zox_sys_in(TunkLink);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, state);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_i(TunkLink, tunk);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(VoxelNodeDirty, dirty);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        entity realm = zox_getv(terrain, RealmLink);
        // byte is_max_depth = depth->value == terrain_depth;
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
        // Home Placeholders
        byte home_wall_id = bricks_id;
        byte home_floor_id = bricks_id;
        byte home_roof_id = bricks_id;
        byte voctree_length = powers_of_two_byte[depth->value];
        int3 chunk_block_position = chunk_position_to_block_position(cposition->value, terrain_depth); // voctree_depth->value);
        // int chunk_position_y = chunk_block_position.y;
        byte3 positionl;
        byte gate_height = 6; // rand_range(4, 8);
        byte wall_height = 8; // rand_range(4, 8);
        byte home_height = 6; // rand_range(4, 8);
        byte hmultiplier = powers_of_two[terrain_depth - depth->value];
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
                if (town_value == zox_town_type_wall) {
                    for (int h = 1; h <= wall_height; h++) {
                        int global_y = height + h;
                        positionl.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (positionl.y >= 0 && positionl.y < voctree_length) {
                            set_clean_VoxelNode(voctree, depth->value, positionl, bricks_id);
                        }
                    }
                }
                // NOTE: Town Gate! has a gap!
                else if (town_value == zox_town_type_gate) {
                    for (int h = gate_height; h <= wall_height; h++) {
                        int global_y = height + h;
                        positionl.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (positionl.y >= 0 && positionl.y < voctree_length) {
                            set_clean_VoxelNode(voctree, depth->value, positionl, bricks_id);
                        }
                    }
                }
                // Home Walls
                else if (town_value == zox_town_type_home_wall) {
                    for (int h = 2; h < home_height; h++) {
                        int global_y = height + h;
                        positionl.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (positionl.y >= 0 && positionl.y < voctree_length) {
                            set_clean_VoxelNode(voctree, depth->value, positionl, home_wall_id);
                        }
                    }
                }
                // Home Door Wall Overhang
                else if (town_value == zox_town_type_home_door) {
                    for (int h = home_height - 1; h < home_height; h++) {
                        int global_y = height + h;
                        positionl.y = (global_y - chunk_block_position.y) / hmultiplier;
                        if (positionl.y >= 0 && positionl.y < voctree_length) {
                            set_clean_VoxelNode(voctree, depth->value, positionl, home_wall_id);
                        }
                    }
                }
                // Roofs
                if (town_value == zox_town_type_home || town_value == zox_town_type_home_door || town_value == zox_town_type_home_wall) {
                    int global_y = height + home_height;
                    positionl.y = (global_y - chunk_block_position.y) / hmultiplier;
                    if (positionl.y >= 0 && positionl.y < voctree_length) {
                        set_clean_VoxelNode(voctree, depth->value, positionl, home_roof_id);
                    }
                }
                // Floors
                if (town_value == zox_town_type_home || town_value == zox_town_type_home_door || town_value == zox_town_type_home_wall) {
                    int global_y = height + 1;
                    positionl.y = (global_y - chunk_block_position.y) / hmultiplier;
                    if (positionl.y >= 0 && positionl.y < voctree_length) {
                        set_clean_VoxelNode(voctree, depth->value, positionl, home_floor_id);
                    }
                }
            }
        }
        write_unlock_VoxelNode(voctree);
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(TownWallsSystem);
