
byte get_sub_node_voxel(const VoxelNode* node, byte3* positionl, byte depth) {
    if (!node) {
        return 0;
    }
    if (depth == 0 || !has_children_VoxelNode(node)) {
        return node->value;
    }
    depth--;
    byte dividor = octree_size(depth); // powers_of_two_byte[depth];
    byte3 positionn = (byte3) {
        positionl->x / dividor,
        positionl->y / dividor,
        positionl->z / dividor
    };
    byte i = byte3_octree_array_index(positionn);
    if (i >= 8) {
        zox_logw("[get_sub_node_voxel] Node Index OOB: %i - depth [%i] - positionl [%ix%ix%i] - positionn [%ix%ix%i] dividor [%i]", i, depth, positionl->x, positionl->y, positionl->z, positionn.x, positionn.y, positionn.z, dividor);
        return node->value;
    }
    byte3_modulus_byte(positionl, dividor); // leftover goes here
    VoxelNode* kids = get_children_VoxelNode(node);
    if (!kids) {
        zox_log_error("node kids null!");
        return node->value;
    }
    return get_sub_node_voxel(&kids[i], positionl, depth);
}

void build_vox_blended(VoxelNode *voxelNode, byte node_depth,
    byte black_voxel_2, byte black_voxel_3,
    byte2 voxel_range, byte2 voxel_range_2,
    byte2 range_blend_1, byte2 range_blend_2, byte vregions) {
    byte grass_height_chance = 6;
    byte length = powers_of_two_byte[node_depth];
    // fill_new_octree(voxelNode, 0, node_depth);  // clear chunk
    voronoi3(voxelNode, node_depth, voxel_range_2, black_voxel_3, vregions);
    vnoise3(voxelNode, node_depth, voxel_range_2, black_voxel_3);  // avoids black ones
    // create new chunk node here, blend two using mask?
    byte grass_position = length - length / 3;
    byte3 voxel_position;
    for (voxel_position.x = 0; voxel_position.x < length; voxel_position.x++) {
        for (voxel_position.z = 0; voxel_position.z < length; voxel_position.z++) {
            byte grass_height = grass_position - rand() % grass_height_chance;
            for (voxel_position.y = grass_height; voxel_position.y < length; voxel_position.y++) {
                byte voxel = voxel_range.x + rand() % (voxel_range.y - voxel_range.x);
                // byte2 set_voxel = (byte2) { voxel, node_depth };
                // byte3 temp_position = voxel_position;
                // set_octree_voxel(voxelNode, &temp_position, &set_voxel, 0);
                set_VoxelNode(voxelNode, node_depth, voxel_position, voxel);
            }
        }
    }
    // add outline between blended parts
    for (voxel_position.x = 0; voxel_position.x < length; voxel_position.x++) {
        for (voxel_position.z = 0; voxel_position.z < length; voxel_position.z++) {
            for (voxel_position.y = 1; voxel_position.y < length - 1; voxel_position.y++) {
                byte3 temp_position_1 = voxel_position;
                byte voxel = get_sub_node_voxel(voxelNode, &temp_position_1, node_depth);
                byte3 temp_position_up = byte3_up(voxel_position);
                byte voxel_up = get_sub_node_voxel(voxelNode, &temp_position_up, node_depth);
                if ((voxel >= range_blend_1.x && voxel <= range_blend_1.y && voxel_up >= range_blend_2.x && voxel_up <= range_blend_2.y) || (voxel >= range_blend_2.x && voxel <= range_blend_2.y && voxel_up >= range_blend_1.x && voxel_up <= range_blend_1.y)) {
                    // byte3 temp_position = voxel_position;
                    // set_octree_voxel(voxelNode, &temp_position, &set_voxel_black_2, 0);
                    set_VoxelNode(voxelNode, node_depth, voxel_position, black_voxel_2);
                }
                byte3 temp_position_down = byte3_down(voxel_position);
                byte voxel_down = get_sub_node_voxel(voxelNode, &temp_position_down, node_depth);
                if ((voxel >= range_blend_1.x && voxel <= range_blend_1.y && voxel_down >= range_blend_2.x && voxel_down <= range_blend_2.y) || (voxel >= range_blend_2.x && voxel <= range_blend_2.y && voxel_down >= range_blend_1.x && voxel_down <= range_blend_1.y)) {
                    //byte3 temp_position = voxel_position;
                    //set_octree_voxel(voxelNode, &temp_position, &set_voxel_black_2, 0);
                    set_VoxelNode(voxelNode, node_depth, voxel_position, black_voxel_2);
                }
                if (voxel_position.x != length - 1) {
                    byte3 temp_position_right = byte3_right(voxel_position);
                    byte voxel_right = get_sub_node_voxel(voxelNode, &temp_position_right, node_depth);
                    if ((voxel >= range_blend_1.x && voxel <= range_blend_1.y && voxel_right >= range_blend_2.x && voxel_right <= range_blend_2.y) || (voxel >= range_blend_2.x && voxel <= range_blend_2.y && voxel_right >= range_blend_1.x && voxel_right <= range_blend_1.y)) {
                        //byte3 temp_position = voxel_position;
                        //set_octree_voxel(voxelNode, &temp_position, &set_voxel_black_2, 0);
                        set_VoxelNode(voxelNode, node_depth, voxel_position, black_voxel_2);
                    }
                }
                if (voxel_position.x != 0) {
                    byte3 temp_position_left = byte3_left(voxel_position);
                    byte voxel_left  = get_sub_node_voxel(voxelNode, &temp_position_left, node_depth);
                    if ((voxel >= range_blend_1.x && voxel <= range_blend_1.y && voxel_left  >= range_blend_2.x && voxel_left  <= range_blend_2.y) || (voxel >= range_blend_2.x && voxel <= range_blend_2.y && voxel_left  >= range_blend_1.x && voxel_left <= range_blend_1.y)) {
                        set_VoxelNode(voxelNode, node_depth, voxel_position, black_voxel_2);
                    }
                }
                if (voxel_position.z != length - 1) {
                    byte3 temp_position_front = byte3_front(voxel_position);
                    byte voxel_front = get_sub_node_voxel(voxelNode, &temp_position_front, node_depth);
                    if ((voxel >= range_blend_1.x && voxel <= range_blend_1.y && voxel_front >= range_blend_2.x && voxel_front <= range_blend_2.y) || (voxel >= range_blend_2.x && voxel <= range_blend_2.y && voxel_front >= range_blend_1.x && voxel_front <= range_blend_1.y)) {
                        set_VoxelNode(voxelNode, node_depth, voxel_position, black_voxel_2);
                    }
                }
                if (voxel_position.z != 0) {
                    byte3 temp_position_back = byte3_back (voxel_position);
                    byte voxel_back  = get_sub_node_voxel(voxelNode, &temp_position_back, node_depth);
                    if ((voxel >= range_blend_1.x && voxel <= range_blend_1.y && voxel_back >= range_blend_2.x && voxel_back <= range_blend_2.y) || (voxel >= range_blend_2.x && voxel <= range_blend_2.y && voxel_back  >= range_blend_1.x && voxel_back  <= range_blend_1.y)) {
                        set_VoxelNode(voxelNode, node_depth, voxel_position, black_voxel_2);
                    }
                }
            }
        }
    }
}
