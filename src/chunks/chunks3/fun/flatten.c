// A voxel octree flattened for the gpu (src/_/octrees/flatten.c, doc/tec/gpu_meshing.md)
//  - tst octree_flatten checks every voxel reads back the same as getv_VoxelNode

// returns the node count, the nodes malloced into *out (free them), 0 on failure
static inline uint flatten_VoxelNode(const VoxelNode* root, uint** out) {
    return flatten_octree(root, sizeof(VoxelNode), octree_type_offset_VoxelNode, 0, out);
}

// the voxel at position, at depth, from flat nodes (air past the edge)
static inline byte flat_voxel_get(const uint* nodes, byte depth, int3 position) {
    return flat_octree_get(nodes, depth, position, 0);
}
