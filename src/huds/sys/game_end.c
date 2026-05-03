zox_sys2(PlayerUIGameEndSystem) {
    const byte delay_end = is_end_game_delays;
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

        // Remove UIs
        entity menu = get_canvas_window(world, canvas->value, zox_window_paused);
        if (zox_valid(menu)) {
            zox_delete(menu); // for second player
        }

        entity menu2 = get_canvas_window(world, canvas->value, zox_window_taskbar);
        if (zox_valid(menu2)) {
            zox_delete(menu2);
        }

        // FadeOut
        if (delay_end) {
            trigger_canvas_fade_transition(world, canvas->value, end_game_delay_fade, 0.8);
        }

    }
} zox_sys_end(PlayerUIGameEndSystem);
