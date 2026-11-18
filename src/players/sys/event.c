// around a player's state hooks, i.e. huds scales the uis they spawn to a split screen's (player_ui_scale_begin)
void (*player_state_event_begin)(ecs*, entity) = NULL;
void (*player_state_event_end)(ecs*, entity) = NULL;

// NOTE: Sets to playing when resuming from pause
void player_state_event_system(iter* it) {
    zox_sys_on_begin();
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
        if (player_state_event_begin) {
            player_state_event_begin(world, e);
        }
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
        if (player_state_event_end) {
            player_state_event_end(world, e);
        }
    }
    zox_sys_on_end();
} zoxd_system(player_state_event_system);