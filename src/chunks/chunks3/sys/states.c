

void voxel_node_dirty_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, VoxelNodeDirty);
        zox_add(e, VoxelNodePostDirty);
    }
    zox_sys_on_end();
} zoxd_system(voxel_node_dirty_system);

void post_voxel_node_dirty_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, VoxelNodePostDirty);
    }
    zox_sys_on_end();
} zoxd_system(post_voxel_node_dirty_system);