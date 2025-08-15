// Queued side updates for propogation
void PropogateQueueSystem(ecs_iter_t *it) {

    zox_sys_begin();
    zox_sys_in(VoxelNode);
    zox_sys_in(LightNodeDepth);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    zox_sys_out(LightNodeQueue);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(VoxelNode, nodev);
        zox_sys_i(LightNodeDepth, depth);
        zox_sys_o(LightNode, root_light);
        zox_sys_o(LightNodeDirty, dirty);
        zox_sys_o(LightNodeQueue, queue);

        (void) root_light;
        (void) depth;
        (void) nodev;

        byte updated = 0;

        while (queue->count) {
            LightNodeUpdate update = r_LightNodeQueue(queue);
            // update.positionl, update. update.value
            (void) update;
        }

        if (updated) {
            dirty->value = zox_dirty_trigger;
        }
    }
} zoxd_system2(PropogateQueueSystem);
