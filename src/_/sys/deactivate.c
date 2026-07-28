/*zox_sys2(DeactivateDelaySystem) {
    //byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(Active);
    zox_sys_out(DeactivateDelay);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Active, active);
        zox_sys_o(DeactivateDelay, delay);
        if (delay->value && zox_current_time - delay->value >= 1) {
            delay->value = 0;
            active->value = 0;
        }
    }
    zox_sys_world();
    for (int i = it->count - 1; i >= 0; i--) {
        zox_sys_e();
        zox_sys_o(DeactivateDelay, delay);
        if (!delay->value) {
            if (zox_tst_remove_deactivates) {
                zox_remove(e, DeactivateDelay);
            }
        }
    }
} zox_sys_end(DeactivateDelaySystem);
*/