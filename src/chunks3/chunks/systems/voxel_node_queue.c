// TODO: Maybe store Depth of these commands? worried about race conditions (as always)

void VoxelNodeQueueSystem(ecs_iter_t *it) {

    // zox_sys_world();
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
        zox_sys_o(VoxelNodeDirty, dirty);
        zox_sys_o(VoxelNodeEdited, edited);

        byte updated = 0;
        SetVoxelTargetData datam = {
            .depth = depth->value,
            .effect_nodes = 1,
        };

        // spin_lock(&queue->lock);
        // while (queue->count) {
        for (size_t i = 0; i < queue->count; i++) {
            VoxelNodeUpdate update = queue->ptr[i];
            datam.voxel = update.value;
            SetVoxelData data2 = {
                .node = node,
                .position = update.positionl,
            };
            VoxelNode* placed = set_voxel(datam, data2);
            if (placed) {
                updated = 1;
            }
            // zox_log("edited voxel: %ix%ix%i", update.positionl.x, update.positionl.y, update.positionl.z);
        }
        // spin_unlock(&queue->lock);

        if (updated) {
            dirty->value = zox_dirty_trigger;
            edited->value = 1;
        }
    }
} zoxd_system2(VoxelNodeQueueSystem);

void VoxelNodeQueueClearSystem(ecs_iter_t *it) {

    zox_sys_begin();
    zox_sys_out(VoxelNodeQueue);

    for (int i = 0; i < it->count; i++) {

        zox_sys_o(VoxelNodeQueue, queue);

        spin_lock(&queue->lock);
        while (queue->count) {
            r_VoxelNodeQueue(queue);
        }
        spin_unlock(&queue->lock);
    }
} zoxd_system2(VoxelNodeQueueClearSystem);

