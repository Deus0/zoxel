void pause_player_ending(ecs *world, entity e) {
    zox_geter_value(e, CanvasLink, entity, canvas);

    if (game_ui_has_taskbar) {
        spawn_taskbar(world, canvas); //, pause_ui_overlay_layer + 1);
    }

    spawn_menu_paused(world, e);
}

// When player state changes, for player UI
zox_sys2(PlayerUIGamePauseSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CanvasLink);
    zox_sys_in(CameraLink);
    zox_sys_out(PlayerPauseEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(CameraLink, camera);
        zox_sys_o(PlayerPauseEvent, pause_event_link);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (state->value != zox_player_state_paused) {
            continue;
        }

        if (!zox_valid(canvas->value)) {
            zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
            continue;
        }

        dispose_menu_game(world, e);
        trigger_canvas_half_fade(world, canvas->value, pause_fade_time, pause_fade_alpha, 1);

        // TODO: Animation End Event - add onto animation created for fader
        entity pause_delay = delay_event(world, &pause_player_ending, e, pause_fade_time);

        if (zox_valid(pause_event_link->value)) {
            zox_delete(pause_event_link->value)
        }
        pause_event_link->value = pause_delay;

        // zox_log("Set Camera Blur [%s] %f", zox_get_name(camera->value), pause_blur);
        zox_set(camera->value, CameraBlur, { pause_blur });

    }
} zox_sys_end(PlayerUIGamePauseSystem);
