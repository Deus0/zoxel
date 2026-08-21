zox_sys2(CooldownSystem) {
    byte dbg_log = zox_log_activations;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WarmupState);
    zox_sys_out(CooldownAt);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(WarmupState, state);
        zox_sys_o(CooldownAt, start);
        if (state->value == zox_dirty_active) {
            start->value = zox_current_time;
            if (dbg_log) {
                zox_sys_e();
                zox_log("[%s]'s Cooldown Started [%0.1f]", zox_get_name(e), zox_current_time);
            }
        }
    }
} zox_sys_end(CooldownSystem);

zox_sys2(CooldownStateSystem) {
    byte dbg_log = zox_log_activations;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CooldownTime);
    zox_sys_out(CooldownAt);
    zox_sys_out(CooldownState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CooldownTime, time);
        zox_sys_o(CooldownAt, at);
        zox_sys_o(CooldownState, state);
        if (!at->value) {
            continue;
        }
        double time_passed = zox_current_time - at->value;
        // zox_log("cooling down %f", time_passed);
        if (time_passed >= time->value) {
            at->value = 0;
            state->value = zox_dirty_trigger;
            if (dbg_log) {
                zox_sys_e();
                zox_log("[%s]'s Cooldown Ended [%f] of [%f]",  zox_get_name(e), time_passed, time->value);
            }
        }
    }
} zox_sys_end(CooldownStateSystem);