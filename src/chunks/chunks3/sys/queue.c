// TODO: Maybe store Depth of these commands? worried about race conditions (as always)
// NOTE: Now we optimize branch nodes in cleanup system
zox_sys2(VoxelNodeQueueSystem) {
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_in(VoxelNodeQueue);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    zox_sys_out(VoxelNodeEdited);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(VoxelNodeQueue, queue);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(VoxelNodeDirty, vdirty);
        zox_sys_o(VoxelNodeEdited, edited);
        byte updated = 0;
        for (size_t i = 0; i < queue->count; i++) {
            VoxelNodeUpdate update = queue->ptr[i];
            if (set_VoxelNode(node, depth->value, update.pos, update.value, 0)) {
                updated = 1;
                // zox_log("edited voxel: %ix%ix%i", update.positionl.x, update.positionl.y, update.positionl.z);
            }
        }
        if (updated) {
            vdirty->value = zox_dirty_trigger;
            edited->value = 1;
        }
    }
} zox_sys_end(VoxelNodeQueueSystem);
