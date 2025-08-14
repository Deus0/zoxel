// Queued side updates for propogation
void PropogateQueueSystem(ecs_iter_t *it) {
    // zox_sys_world();
    zox_sys_begin();

    zox_sys_in(VoxelNode);
    zox_sys_in(LightNodeDepth);
    zox_sys_out(LightNode);
    zox_sys_out(LightNodeDirty);
    zox_sys_out(LightNodeQueue);


    for (int i = 0; i < it->count; i++) {

        zox_sys_i(VoxelNode, nodev);
        zox_sys_i(LightNodeDepth, depth);
        zox_sys_o(LightNode, nodel);
        zox_sys_o(LightNodeDirty, dirty);
        zox_sys_o(LightNodeQueue, queue);

        byte updated = 0;

        // spin_lock(&queue->lock);
        while (queue->count) {
            LightNodeUpdate update = r_LightNodeQueue(queue);
            // update.positionl, update. update.value

        }
        // spin_unlock(&queue->lock);

        if (updated) {
            dirty->value = zox_dirty_trigger;
        }
    }
} zoxd_system2(PropogateQueueSystem);
