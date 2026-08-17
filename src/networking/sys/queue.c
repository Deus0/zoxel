zox_sys2(PacketQueueSystem) {
    byte dbg_log = 1;
    zox_sys_begin();
    zox_sys_out(PacketQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(PacketQueue, queue);
        for (int j = queue->count - 1; j >= 0; j--) {
            PacketUpdate* update = &queue->ptr[j];
            if (update->state == zox_packet_queue_end) {
                remove_at_PacketQueue(queue, j);
                if (dbg_log > 2) {
                    zox_log("Packet Queue Removed [%i]", j);
                }
                continue;
            }
            update->state++;
            if (update->state == zox_packet_queue_process) {
                if (dbg_log) {
                    zox_log("Packet Queue Processed [%i]: %s", j, zox_get_packet_name(update->type));
                }
            }
        }
    }
} zox_sys_end(PacketQueueSystem);