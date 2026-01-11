// Reset our deltas
zox_sys2(SystemDeltaLogResetSystem) {
    zox_sys_begin();
    zox_sys_out(SystemDelta);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(SystemDelta, delta);
        delta->value = 0;
    }
} zox_sys_end(SystemDeltaLogResetSystem);
