void vnoise3(VoxelNode* voctree, byte depth, byte2 voxel_range, byte black_voxel) {
    byte length = powers_of_two_byte[depth];
    byte3 size = byte3_single(length);
    byte3 position;
    for (position.x = 0; position.x < size.x; position.x++) {
        for (position.y = 0; position.y < size.y; position.y++) {
            for (position.z = 0; position.z < size.z; position.z++) {
                // skip outline parts
                if (getv_VoxelNode(voctree, depth, position) == black_voxel) {
                    continue;
                }
                byte value = voxel_range.x + rand() % (voxel_range.y - voxel_range.x);
                set_VoxelNode(voctree, depth, position, value);
            }
        }
    }
}

void vnoise3_spray_side(VoxelNode* voctree, byte depth, byte value, byte2 penetrations, byte direction) {
    // NOTE: No need to spray at lower depths
    if (depth <= 1) {
        return;
    }
    byte length = powers_of_two_byte[depth];
    byte3 size = byte3_single(length);
    if (penetrations.x >= length) {
        penetrations.x = length - 1;
    }
    if (penetrations.y > length) {
        penetrations.y = length;
    }
    byte3 position;
    if (direction == direction_up) {
        for (position.x = 0; position.x < size.x; position.x++) {
            for (position.z = 0; position.z < size.z; position.z++) {
                byte penetration = rand_range(penetrations.x, penetrations.y);
                for (byte p = 0; p < penetration; p++) {
                    position.y = size.y - 1 - p;
                    int rando = rand() % 1000;
                    if (rando > 600) {
                        break;
                    }
                    set_VoxelNode(voctree, depth, position, value);
                }
            }
        }
    } else if (direction == direction_left || direction == direction_right) {
        for (position.y = 0; position.y < size.y; position.y++) {
            for (position.z = 0; position.z < size.z; position.z++) {
                byte penetration = rand_range(penetrations.x, penetrations.y);
                for (byte p = 0; p < penetration; p++) {
                    if (direction == direction_right) {
                        position.x = p;
                    } else {
                        position.x = size.x - 1 - p;
                    }
                    int rando = rand() % 1000;
                    if (rando > 300) {
                        break;
                    }
                    set_VoxelNode(voctree, depth, position, value);
                }
            }
        }
    } else if (direction == direction_back || direction == direction_front) {
        for (position.y = 0; position.y < size.y; position.y++) {
            for (position.x = 0; position.x < size.x; position.x++) {
                byte penetration = rand_range(penetrations.x, penetrations.y);
                for (byte p = 0; p < penetration; p++) {
                    if (direction == direction_front) {
                        position.z = p;
                    } else {
                        position.z = size.z - 1 - p;
                    }
                    int rando = rand() % 1000;
                    if (rando > 300) {
                        break;
                    }
                    set_VoxelNode(voctree, depth, position, value);
                }
            }
        }
    } else {
        zox_logw("[vnoise3_spray_side] Unsupported direction [%i]", direction);
    }
}
