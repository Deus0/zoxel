void build_vox_soil(VoxelNode *voxelNode, byte depth, byte2 voxel_range, byte black_voxel_3, byte vregions) {
    voronoi3(voxelNode, depth, voxel_range, black_voxel_3, vregions);
    vnoise3(voxelNode, depth, voxel_range, black_voxel_3);
}
