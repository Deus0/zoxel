// TODO: Maybe store Depth of these commands? worried about race conditions (as always)
// NOTE: Now we optimize branch nodes in cleanup system
zox_sys2(VoxelNodeQueueSystem) {
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_in(VoxelNodeQueue);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    zox_sys_out(VoxelNodeEdited);
    zox_sys_out(VoxelDropQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(VoxelNodeQueue, queue);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(VoxelNodeDirty, vdirty);
        zox_sys_o(VoxelNodeEdited, edited);
        zox_sys_o(VoxelDropQueue, drops);
        byte updated = 0;
        for (size_t i = 0; i < queue->count; i++) {
            VoxelNodeUpdate update = queue->ptr[i];
            write_lock_VoxelNode(voctree);
            // TODO: Grab the value when setting instead
            byte old_value = getv_VoxelNode(voctree, depth->value, update.pos);
            if (set_VoxelNode(voctree, depth->value, update.pos, update.value)) {
                updated = 1;
                if (old_value) {
                    a_VoxelDropQueue(drops, (VoxelDropElement) { .value = old_value, .pos = update.pos });
                }
                // zox_log("edited voxel: %ix%ix%i", update.positionl.x, update.positionl.y, update.positionl.z);
            }
            write_unlock_VoxelNode(voctree);
        }
        if (updated) {
            vdirty->value = zox_dirty_trigger;
            edited->value = 1;
        }
    }
} zox_sys_end(VoxelNodeQueueSystem);
