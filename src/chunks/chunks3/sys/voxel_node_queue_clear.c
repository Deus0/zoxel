zox_sys2(VoxelNodeQueueClearSystem) {
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
} zox_sys_end(VoxelNodeQueueClearSystem);
