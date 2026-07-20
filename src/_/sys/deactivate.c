
zox_sys2(DeactivateDelaySystem) {
    //byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(Active);
    zox_sys_out(DeactivateDelay);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Active, active);
        zox_sys_o(DeactivateDelay, delay);
        if (!delay->value) {
            continue;
        }
        delay->value--;
        if (!delay->value) {
            active->value = 0;
        }
    }
} zox_sys_end(DeactivateDelaySystem);