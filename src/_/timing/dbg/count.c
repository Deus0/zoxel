

// Reset our deltas
zox_sys2(SystemProcessedResetSystem) {
    zox_sys_begin();
    zox_sys_out(SystemProcessed);
    zox_sys_out(SystemProcessedCache);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(SystemProcessed, count);
        zox_sys_o(SystemProcessedCache, cache);
        cache->value = count->value;
        count->value = 0;
    }
} zox_sys_end(SystemProcessedResetSystem);
