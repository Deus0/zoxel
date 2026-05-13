// uses chunk above for air check
// chunk_above used purely for top of chunk checks
byte find_position_on_ground(const VoxelNode *chunk, const VoxelNode *chunk_above, byte depth, byte2 input) {
    if (chunk == NULL) {
        return 255;
    }
    byte length = powers_of_two_byte[depth];
    // find ground from tallest point
    // NOTE: Special case for top of chunks
    if (chunk_above) {
        byte voxel_up = getv_VoxelNode(chunk_above, (byte3) { input.x, 0, input.y }, depth);
        byte voxel_down = getv_VoxelNode(chunk, (byte3) { input.x, length - 1, input.y }, depth);
        if (!voxel_up && voxel_down) { // can stand on voxel
            return length;
        }
    }
    for (byte y = length - 1; y >= 1; y--)
    {
        byte voxel_up = getv_VoxelNode(chunk, (byte3) { input.x, y, input.y }, depth);
        byte voxel_down = getv_VoxelNode(chunk, (byte3) { input.x, y - 1, input.y }, depth);
        if (!voxel_up && voxel_down) { // can stand on voxel
            return y;
        }
    }
    return 255;
}

byte find_random_position_on_ground(const VoxelNode* chunk, const VoxelNode* chunk_above, byte depth, byte max_checks, byte3* position) {
    byte length = powers_of_two_byte[depth];
    byte checks_count = 0;
    while (checks_count < max_checks) {
        byte2 positionxz = (byte2) { rand() % length, rand() % length        };
        byte y = find_position_on_ground(chunk, chunk_above, depth, positionxz);
        if (y != 255) {
            *position = (byte3) { positionxz.x, y, positionxz.y };
            return 1;
        }
        checks_count++;
    }
    return 0;
}
