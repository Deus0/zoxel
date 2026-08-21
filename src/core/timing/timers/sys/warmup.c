// If Not cooling or warming, start warmup state
zox_sys2(WarmupSystem) {
    byte dbg_log = zox_log_activations;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ActivateBegin);
    zox_sys_in(CooldownAt);
    zox_sys_out(WarmupAt);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ActivateBegin, begin);
        zox_sys_i(CooldownAt, cooling);
        zox_sys_o(WarmupAt, start);
        if (!cooling->value && !start->value && begin->value == zox_dirty_active) {
            start->value = zox_current_time;
            if (dbg_log) {
                zox_sys_e();
                zox_log("[%s]'s Warmup Started [%0.1f]", zox_get_name(e), zox_current_time);
            }
        }
    }
} zox_sys_end(WarmupSystem);

zox_sys2(WarmupStateSystem) {
    byte dbg_log = zox_log_activations;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WarmupTime);
    zox_sys_out(WarmupAt);
    zox_sys_out(WarmupState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(WarmupTime, time);
        zox_sys_o(WarmupAt, warmup);
        zox_sys_o(WarmupState, state);
        if (!warmup->value) {
            continue;
        }
        double time_passed = zox_current_time - warmup->value;
        // zox_log("warmup started %f", time_passed);
        if (time_passed >= time->value) {
            warmup->value = 0;
            state->value = zox_dirty_trigger;
            if (dbg_log) {
                zox_sys_e();
                zox_log("[%s]'s Warmup Ended [%f] of [%f]",  zox_get_name(e), time_passed, time->value);
            }
        }
    }
} zox_sys_end(WarmupStateSystem);