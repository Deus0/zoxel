void vnoise3(VoxelNode* voctree, byte depth, byte2 voxel_range, byte black_voxel) {

    byte length = powers_of_two_byte[depth];
    byte3 size = byte3_single(length);
    byte3 position;
    for (position.x = 0; position.x < size.x; position.x++) {
        for (position.y = 0; position.y < size.y; position.y++) {
            for (position.z = 0; position.z < size.z; position.z++) {
                // skip outline parts
                if (get_value_VoxelNode(voctree, depth, position, 0) == black_voxel) {
                    continue;
                }
                /*const int rando = rand() % 1000;
                if (rando > 300) {
                    continue;
                }*/
                byte value = voxel_range.x + rand() % (voxel_range.y - voxel_range.x);

                set_VoxelNode(voctree, depth, position, value, 0);
            }
        }
    }
}

void vnoise3_spray_side(VoxelNode* voctree, byte depth, byte value, byte min_penetration, byte max_penetration, byte direction) {

    byte length = powers_of_two_byte[depth];
    byte3 size = byte3_single(length);
    byte3 position;
    if (direction == direction_up) {
        for (position.x = 0; position.x < size.x; position.x++) {
            for (position.z = 0; position.z < size.z; position.z++) {
                byte penetration = min_penetration + (rand() % (max_penetration - min_penetration));
                for (int p = 0; p < penetration; p++) {
                    position.y = size.y - 1 - p;

                    int rando = rand() % 1000;
                    if (rando > 600) {
                        continue;
                    }

                    set_VoxelNode(voctree, depth, position, value, 0);
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

                    int rando = rand() % 1000;
                    if (rando > 300) {
                        continue;
                    }

                    set_VoxelNode(voctree, depth, position, value, 0);
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

                    int rando = rand() % 1000;
                    if (rando > 300) {
                        continue;
                    }

                    set_VoxelNode(voctree, depth, position, value, 0);
                }
            }
        }
    } else {
        zox_logw("[vnoise3_spray_side] Unsupported direction [%i]", direction);
    }
}
