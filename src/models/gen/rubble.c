void build_vox_rubble(VoxelNode *voctree, byte depth, byte2 voxel_range, int rubble_count, byte rubble_height) {
    byte length = powers_of_two_byte[depth];
    byte3 position;
    fill_octree(voctree, 0, depth);  // clear chunk
    for (int j = 0; j < rubble_count; j++) {
        position.x = rand() % length;
        position.z = rand() % length;
        for (int k = 0; k < length; k++) {
            position.y = k;
            if (get_value_VoxelNode(voctree, depth, position, 0) == 0) {
                break;
            }
        }
        if (position.y > rubble_height) {
            continue;
        }
        byte value = voxel_range.x + rand() % (voxel_range.y - voxel_range.x);
        set_VoxelNode(voctree, depth, position, value, 0);
    }
}
