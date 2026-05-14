// TODO: Pass in CanGroup block byte tags, so we dont group some blocks like Grass
zox_sys2(VoxelOctreeCleanupSystem) {
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_out(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxelNodeDirty, dirty);
        zox_sys_o(VoxelNode, voctree);
        if (dirty->value == zox_dirty_active) {
            optimize_VoxelNode(voctree);
            reduce_VoxelNode(voctree);
        }
    }
} zox_sys_end(VoxelOctreeCleanupSystem);
