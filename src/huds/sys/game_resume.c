void resume_player_delayed(ecs *world, entity player) {
    zox_geter_value(player, CharacterLink, entity, character);

    if (!zox_alive(character)) {
        return;
    }

    if (local_mouse) {
        zox_set(local_mouse, MouseLock, { 1 });
    }
    zox_set(character, DisableMovement, { 0 });
    spawn_in_game_ui(world, player);
}

zox_sys2(PlayerResumeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CanvasLink);
    zox_sys_in(CameraLink);
    zox_sys_out(PlayerState);
    zox_sys_out(PlayerStateDirty);
    zox_sys_out(PlayerPauseEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(CameraLink, camera);
        zox_sys_o(PlayerState, state);
        zox_sys_o(PlayerStateDirty, dirty);
        zox_sys_o(PlayerPauseEvent, pause_event_link);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (state->value != zox_player_state_resuming) {
            continue;
        }

        entity menu = get_canvas_window(world, canvas->value, zox_window_paused);
        // find_child_with_tag(canvas->value, MenuPaused, menu_paused);
        // entity menu = find_child_with_tag_recursive(world, canvas->value, zox_id(MenuPaused));
        if (zox_valid(menu)) {
            zox_delete(menu);
        }

        // find_child_with_tag(canvas->value, Taskbar, taskbar);
        menu = find_child_with_tag_recursive(world, canvas->value, zox_id(Taskbar));
        if (zox_valid(menu)) {
            zox_delete(menu);
        }

        trigger_canvas_half_fade(world, canvas->value, pause_fade_time, pause_fade_alpha, 0);

        entity pause_event = delay_event(world, &resume_player_delayed, e, pause_fade_time);

        if (zox_valid(pause_event_link->value)) {
            zox_delete(pause_event_link->value)
        }

        // setters
        pause_event_link->value = pause_event;
        state->value = zox_player_state_playing;
        dirty->value = zox_dirty_trigger;

        zox_set(camera->value, CameraBlur, { 0 });
    }
} zox_sys_end(PlayerResumeSystem);
