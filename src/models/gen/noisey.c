

void vnoise3(
    VoxelNode* voctree,
    byte depth,
    byte2 voxel_range,
    byte black_voxel,
    uint* random_state)
{
    byte length = powers_of_two_byte[depth];

    byte3 position;

    for (position.x = 0; position.x < length; position.x++) {
        for (position.y = 0; position.y < length; position.y++) {
            for (position.z = 0; position.z < length; position.z++) {
                if (getv_VoxelNode(voctree, depth, position) == black_voxel) {
                    continue;
                }
                byte value = noise_rand_range(
                    random_state,
                    voxel_range.x,
                    voxel_range.y);
                set_voxel_safe(
                    voctree,
                    depth,
                    position,
                    value);
            }
        }
    }
}

/*void vnoise3(
    VoxelNode* voctree,
    byte depth,
    byte2 voxel_range,
    byte black_voxel)
{
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
                set_voxel_safe(
                    voctree,
                    depth,
                    position,
                    value);
            }
        }
    }
}*/