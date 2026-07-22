zox_sys2(VoxelNodeQueueClearSystem) {
    zox_sys_begin();
    zox_sys_out(VoxelNodeQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(VoxelNodeQueue, queue);
        if (locks_enabled) spin_lock(&queue->lock);
        while (queue->count) {
            remove_VoxelNodeQueue(queue);
        }
        if (locks_enabled) {
            spin_unlock(&queue->lock);
        }
    }
} zox_sys_end(VoxelNodeQueueClearSystem);

/*zox_sys2(VoxelDropQueueClearSystem) {
    zox_sys_begin();
    zox_sys_out(VoxelDropQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(VoxelDropQueue, queue);
        if (locks_enabled) spin_lock(&queue->lock);
        while (queue->count) {
            remove_VoxelDropQueue(queue);
        }
        if (locks_enabled) {
            spin_unlock(&queue->lock);
        }
    }
} zox_sys_end(VoxelDropQueueClearSystem);*/

