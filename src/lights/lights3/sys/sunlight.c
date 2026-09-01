// NOTE: Sunlight now just creates queue entries
void sunlight_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_out(SunlightQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeDepth, depth);
        zox_sys_o(SunlightQueue, queue);
        short length = octree_size(depth->value);
        if (dbg_log) {
            zox_log("[%s] Queue Topmost Sunbeams Light [%i] Depth [%i]", zox_sys_e_name, sunlight, depth->value);
        }
        byte3 position;
        position.y = length - 1;
        for (position.x = 0; position.x < length; position.x++) {
            for (position.z = 0; position.z < length; position.z++) {
                if (locks_enabled) {
                    spin_lock(&queue->lock);
                }
                a_SunlightQueue(queue, (SunlightUpdate) {
                    .pos = position,
                    .light = sunlight,
                    .depth = depth->value
                });
                if (locks_enabled) {
                    spin_unlock(&queue->lock);
                }
            }
        }
        zox_remove(e, GenerateSunlight);
    }
    zox_sys_on_end();
} zoxd_system(sunlight_system);