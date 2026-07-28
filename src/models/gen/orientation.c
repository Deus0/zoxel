void build_vox_orientation_test(VoxelNode *voctree, byte depth, byte inside, byte neg_x, byte pos_x, byte neg_y, byte pos_y, byte neg_z, byte pos_z) {
    short vlength = octree_size(depth);
    if (vlength == 1) {
        set_VoxelNode(voctree, depth, byte3_zero, inside);
        return;
    }
    byte3 size = byte3_single(vlength);
    byte3 pos;
    for (pos.y = 0; pos.y < size.y; pos.y++) {
        for (pos.x = 0; pos.x < size.x; pos.x++) {
            for (pos.z = 0; pos.z < size.z; pos.z++) {
                byte voxel = inside;
                if (pos.x == 0)
                    voxel = neg_x;
                else if (pos.x == size.x - 1)
                    voxel = pos_x;
                else if (pos.y == 0)
                    voxel = neg_y;
                else if (pos.y == size.y - 1)
                    voxel = pos_y;
                else if (pos.z == 0)
                    voxel = neg_z;
                else if (pos.z == size.z - 1)
                    voxel = pos_z;
                set_voxel_safe(voctree, depth, pos, voxel);
            }
        }
    }
}