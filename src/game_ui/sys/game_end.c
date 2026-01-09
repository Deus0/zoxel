void PlayerUIGameEndSystem(iter *it) {
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

        // Remove UIs
        find_child_with_tag(canvas->value, MenuPaused, menu_paused);
        if (zox_valid(menu_paused)) {
            zox_delete(menu_paused); // for second player
        }

        find_child_with_tag(canvas->value, Taskbar, taskbar);
        if (zox_valid(taskbar)) {
            zox_delete(taskbar);
        }

        // FadeOut
        if (delay_end) {
            trigger_canvas_fade_transition(world, canvas->value, end_game_delay_fade, 0.8);
        }

    }
} zoxd_system2(PlayerUIGameEndSystem);