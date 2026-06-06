//
zox_sys2(PlayerUIResumeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CanvasLink);
    zox_sys_in(CameraLink);
    // zox_sys_in(CharacterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(CameraLink, camera);
        // zox_sys_i(CharacterLink, character);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(canvas->value)) {
            zox_logw("Canvas is missing from Player [PlayerResumeSystem]");
            continue;
        }
        if (state->value == zox_player_state_resume_begin) {
            entity pause_menu = zox_get_child_by_id(world, canvas->value, zox_id(MenuPaused));
            if (zox_valid(pause_menu)) {
                zox_delete(pause_menu);
            }
            entity taskbar = zox_get_child_by_id(world, canvas->value, zox_id(Taskbar));
            if (zox_valid(taskbar)) {
                zox_delete(taskbar);
            }
            trigger_canvas_half_fade(world, canvas->value, pause_fade_time, pause_fade_alpha, 0);
        } else if (state->value == zox_player_state_play_begin) {
            zox_set(camera->value, CameraBlur, { 0 });
            // spawn_in_game_ui(world, e);
            /*if (zox_valid(character->value)) {
                zox_set(character, DisableMovement, { 0 });
            }*/
            /*if (local_mouse) {
                zox_set(local_mouse, MouseLock, { 1 });
            }*/
        }
        /*entity pause_event = delay_event(world, &resume_player_delayed, e, pause_fade_time);
        if (zox_valid(pause_event_link->value)) {
            zox_delete(pause_event_link->value)
        }
        // setters
        pause_event_link->value = pause_event;*/
    }
} zox_sys_end(PlayerUIResumeSystem);
