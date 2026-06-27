zox_sys2(PlayerUIGameEndSystem) {
    byte dbg_log = 1;
    byte delay_end = is_end_game_delays;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CanvasLink, canvas);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (state->value != zox_player_state_main_menu) {
            continue;
        }
        if (!zox_valid(canvas->value)) {
            zox_logw("Canvas is missing from Player [PlayerUIGameEndSystem]");
            continue;
        }
        // Remove any uis
        entity uis[zox_children_capacity];
        uint length = zox_get_children_by_id(world, canvas->value, uis, zox_children_capacity, zox_id(Window));
        for (uint j = 0; j < length; j++) {
            entity e2 = uis[j];
            if (dbg_log) {
                zox_log("Destroying UI [%s]", zox_get_name(e2));
            }
            zox_delete(e2); // for second player
        }
        // FadeOut
        if (delay_end) {
            trigger_canvas_fade_transition(world, canvas->value, end_game_delay_fade, 0.8);
        }
    }
} zox_sys_end(PlayerUIGameEndSystem);
