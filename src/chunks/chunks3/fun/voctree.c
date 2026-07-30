// used by physics and raycasting
// i think const was the issue

void fill_octree(VoxelNode* voctree, byte voxel, byte depth) {
    if (!voctree) {
        return;
    }
    voctree->value = voxel;
    if (depth == 0) {
        return;
    }
    depth--;
    open_one_VoxelNode(voctree);
    VoxelNode* kids = (VoxelNode*) voctree->ptr;
    for (byte i = 0; i < octree_length; i++) {
        fill_octree(&kids[i], voxel, depth);
    }
}
