// TODO: Pass in CanGroup block byte tags, so we dont group some blocks like Grass
// NOTE: We might need a post optimize tag - for mesh updates etc
void voxel_octree_optimize_system(iter* it) {
    zox_sys_on_begin();
    // zox_sys_world();
    zox_sys_begin();
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeLock);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_o(VoxelNode, voxels);
        zox_sys_o(VoxelNodeLock, lock);
        spin_lock(&lock->value);
        optimize_reduce_VoxelNode(voxels);
        spin_unlock(&lock->value);
        // zox_add(e, VoxelNodePostDirty);
        // zox_remove(e, VoxelNodeDirty);
    }
    zox_sys_on_end();
} zoxd_system(voxel_octree_optimize_system);
