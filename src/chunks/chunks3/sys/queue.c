// TODO: Maybe store Depth of these commands? worried about race conditions (as always)
// NOTE: Now we optimize branch nodes in cleanup system
// NOTE: This wll use a queue to do batches of octtree updates to voxels
zox_sys2(VoxelUpdateQueueSystem) {
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_out(VoxelNodeQueue);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    zox_sys_out(VoxelNodeEdited);
    // zox_sys_out(VoxelDropQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(NodeDepth, depth);
        zox_sys_o(VoxelNodeQueue, queue);
        zox_sys_o(VoxelNode, voxels);
        zox_sys_o(VoxelNodeDirty, dirty);
        zox_sys_o(VoxelNodeEdited, edited);
        // zox_sys_o(VoxelDropQueue, drops);
        byte updated = 0;
        for (int j = queue->count - 1; j >= 0; j--) {
            VoxelNodeUpdate* update = &queue->ptr[j];
            if (update->state == zox_voxel_queue_post) {
                update->state = zox_voxel_queue_end;
                continue;
            } else if (update->state == zox_voxel_queue_end) {
                remove_at_VoxelNodeQueue(queue, j);
                continue;
            }
            update->old_value = getv_VoxelNode(voxels, depth->value, update->position);
            write_lock_VoxelNode(voxels);
            // TODO: Grab the value when setting instead
            if (set_VoxelNode(voxels, depth->value, update->position, update->value)) {
                updated = 1;
                /*if (old_value) {
                    a_VoxelDropQueue(drops, (VoxelDropElement) { .value = old_value, .pos = update.pos });
                }*/
                // zox_log("edited voxel: %ix%ix%i", update.positionl.x, update.positionl.y, update.positionl.z);
            }
            write_unlock_VoxelNode(voxels);
            update->state = zox_voxel_queue_post;
        }
        if (updated) {
            dirty->value = zox_dirty_trigger;
            edited->value = 1;
        }
    }
} zox_sys_end(VoxelUpdateQueueSystem);
