// NOTE: Sets to playing when resuming from pause
zox_sys2(PlayerStateEventSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerStateEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerStateEvent, event);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        /*if (event->value == NULL) {
            zox_loge("PlayerStateEvent is null");
            continue;
        }*/
        for (int j = 0; j < event->count; j++) {
            if (event->value[j].fun == NULL) {
                zox_loge("Player [%s] StateEvent Null [%i]",
                    zox_sys_e_name,
                    j);
                continue;
            }
            if (dbg_log) {
                zox_log("Calling PlayerStateEvent [%i] ptr [%p]",
                    j, (void *) event->value[j].fun);
            }
            event->value[j].fun(
                world,
                e,
                state->value);
        }
    }
} zox_sys_end(PlayerStateEventSystem);