// Reset our deltas
void system_processed_reset_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_out(SystemProcessed);
    zox_sys_out(SystemProcessedCache);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(SystemProcessed, count);
        zox_sys_o(SystemProcessedCache, cache);
        cache->value = count->value;
        count->value = 0;
    }
    zox_sys_on_end();
} zoxd_system(system_processed_reset_system);
