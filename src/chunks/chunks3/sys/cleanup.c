// TODO: Pass in CanGroup block byte tags, so we dont group some blocks like Grass
zox_sys2(VoxelOctreeOptimizeSystem) {
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_out(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxelNodeDirty, dirty);
        zox_sys_o(VoxelNode, voctree);
        if (dirty->value == zox_dirty_end) {
            write_lock_VoxelNode(voctree);
            optimize_VoxelNode(voctree);
            reduce_VoxelNode(voctree);
            write_unlock_VoxelNode(voctree);
        }
    }
} zox_sys_end(VoxelOctreeOptimizeSystem);
