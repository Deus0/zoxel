void LightNodeQueueSystem(ecs_iter_t *it) {
    // zox_sys_world();
    zox_sys_begin();

    zox_sys_in(VoxelNode);
    zox_sys_in(LightNodeDepth);
    zox_sys_out(LightNodeQueue);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(VoxelNode, nodev);
        zox_sys_i(LightNodeDepth, depth);
        zox_sys_o(LightNodeQueue, queue);
        zox_sys_o(LightNode, nodel);
        zox_sys_o(LightNodeDirty, dirty);

        /*byte updated = 0;
        SetVoxelTargetData datam = {
            .depth = depth->value,
            .effect_nodes = 1,
        };

        spin_lock(&queue->lock);
        while (queue->count) {
            VoxelNodeUpdate update = r_VoxelNodeQueue(queue);
            datam.voxel = update.value;
            SetVoxelData data2 = {
                .node = node,
                .position = update.positionl,
            };
            VoxelNode* placed = set_voxel(datam, data2);
            if (placed) {
                updated = 1;
            }
        }
        spin_unlock(&queue->lock);

        if (updated) {
            dirty->value = zox_dirty_trigger;
        }*/
    }
} zoxd_system2(LightNodeQueueSystem);
