// Reset our deltas
zox_sys2(SystemDeltaCacheSystem) {
    zox_sys_begin();
    zox_sys_out(SystemDelta);
    zox_sys_out(SystemDeltaCache);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(SystemDelta, delta);
        zox_sys_o(SystemDeltaCache, cache);
        cache->value = delta->value;
        delta->value = 0;
    }
} zox_sys_end(SystemDeltaCacheSystem);
