zox_sys2(ActivateSystem) {
    zox_sys_begin();
    zox_sys_in(WarmupState);
    zox_sys_out(Activate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(WarmupState, trigger);
        zox_sys_o(Activate, activate);

        if (trigger->value == zox_dirty_active) {
            activate->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(ActivateSystem);