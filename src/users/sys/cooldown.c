zox_sys2(CooldownSystem) {
    zox_sys_begin();
    zox_sys_in(WarmupState);
    zox_sys_out(CooldownAt);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(WarmupState, state);
        zox_sys_o(CooldownAt, start);
        if (state->value == zox_dirty_active) {
            start->value = zox_current_time;
            // zox_log("cooldown started");
        }
    }
} zox_sys_end(CooldownSystem);

zox_sys2(CooldownStateSystem) {
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
        double passed = zox_current_time - at->value;
        // zox_log("cooling down %f", passed);
        if (passed >= time->value) {
            at->value = 0;
            state->value = zox_dirty_trigger;
            // zox_log("cooldown ended %f", passed);
        }
    }
} zox_sys_end(CooldownStateSystem);