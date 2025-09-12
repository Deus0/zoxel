void vnoise3(
    VoxelNode *node,
    const byte node_depth,
    const byte2 voxel_range,
    const byte black_voxel
) {
    const byte length = powers_of_two_byte[node_depth];
    byte3 size = byte3_single(length);
    byte3 position;
    for (position.x = 0; position.x < size.x; position.x++) {
        for (position.y = 0; position.y < size.y; position.y++) {
            for (position.z = 0; position.z < size.z; position.z++) {
                // skip outline parts
                byte3 temp_position = position;
                if (get_sub_node_voxel(node, &temp_position, node_depth) == black_voxel) {
                    continue;
                }
                /*const int rando = rand() % 1000;
                if (rando > 300) {
                    continue;
                }*/
                const byte value = voxel_range.x + rand() % (voxel_range.y - voxel_range.x);
                //byte2 set_voxel = (byte2) { voxel, node_depth };
                //byte3 node_position = position;
                //set_octree_voxel(node, &node_position, &set_voxel, 0);
                set_voxelc(
                    node,
                    node_depth,
                    position,
                    value,
                    0);
            }
        }
    }
}

void vnoise3_spray_side(
    VoxelNode *node,
    const byte node_depth,
    const byte value,
    const byte min_penetration,
    const byte max_penetration,
    const byte direction
) {
    const byte length = powers_of_two_byte[node_depth];
    byte3 size = byte3_single(length);
    byte3 position;
    if (direction == direction_up) {
        for (position.x = 0; position.x < size.x; position.x++) {
            for (position.z = 0; position.z < size.z; position.z++) {
                byte penetration = min_penetration + (rand() % (max_penetration - min_penetration));
                for (int p = 0; p < penetration; p++) {
                    position.y = size.y - 1 - p;
                    const int rando = rand() % 1000;
                    if (rando > 600) {
                        continue;
                    }
                    set_voxelc(
                        node,
                        node_depth,
                        position,
                        value,
                        0
                    );
                }
            }
        }
    } else if (direction == direction_left || direction == direction_right) {
        for (position.y = 0; position.y < size.y; position.y++) {
            for (position.z = 0; position.z < size.z; position.z++) {
                byte penetration = min_penetration + (rand() % (max_penetration - min_penetration));
                for (int p = 0; p < penetration; p++) {
                    if (direction == direction_right) {
                        position.x = p;
                    } else {
                        position.x = size.x - 1 - p;
                    }
                    const int rando = rand() % 1000;
                    if (rando > 300) {
                        continue;
                    }
                    set_voxelc(
                        node,
                        node_depth,
                        position,
                        value,
                        0
                    );
                }
            }
        }
    } else if (direction == direction_back || direction == direction_front) {
        for (position.y = 0; position.y < size.y; position.y++) {
            for (position.x = 0; position.x < size.x; position.x++) {
                byte penetration = min_penetration + (rand() % (max_penetration - min_penetration));
                for (int p = 0; p < penetration; p++) {
                    if (direction == direction_front) {
                        position.z = p;
                    } else {
                        position.z = size.z - 1 - p;
                    }
                    const int rando = rand() % 1000;
                    if (rando > 300) {
                        continue;
                    }
                    set_voxelc(
                        node,
                        node_depth,
                        position,
                        value,
                        0
                    );
                }
            }
        }
    } else {
        zox_logw("[vnoise3_spray_side] Unsupported direction [%i]", direction);
    }
}