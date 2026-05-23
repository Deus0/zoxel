void random_fill_octree(VoxelNode* node, byte voxel, byte depth) {
    node->value = voxel;
    if (depth > 0) {
        depth--;
        open_VoxelNode(node);
        VoxelNode* kids = get_children_VoxelNode(node);
        for (int i = 0; i < octree_length; i++) {
            random_fill_octree(&kids[i], voxel, depth);
        }
    } else {
        if (rand() % 101 >= fill_octree_random_rate) {
            node->value = 0;
        } else if (rand() % 101 >= fill_octree_random_rate2) {
            node->value = 2;
        } else if (rand() % 101 >= fill_octree_random_rate3) {
            node->value = 3;
        }
    }
}

zox_sys2(NoiseVoxelNodeSystem) {
    zox_change_check()
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_out(ChunkDirty);
    zox_sys_out(VoxelNode);
    zox_sys_out(Generate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(NodeDepth, nodeDepth);
        zox_sys_o(ChunkDirty, chunkDirty);
        zox_sys_o(VoxelNode, voxelNode);
        zox_sys_o(Generate, generateChunk);
        if (!generateChunk->value || chunkDirty->value) {
            continue;
        }
        random_fill_octree(voxelNode, 1, nodeDepth->value);
        generateChunk->value = 0;
        chunkDirty->value = 1;
    }
} zox_sys_end(NoiseVoxelNodeSystem);
