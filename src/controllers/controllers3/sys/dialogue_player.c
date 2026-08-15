// NOTE: This handles player dialogue state changes
zox_sys2(PlayerDialogueSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CharacterLink);
    zox_sys_in(CameraLink);
    zox_sys_in(CanvasLink);
    zox_sys_out(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CharacterLink, character);
        zox_sys_i(CameraLink, camera);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_o(PlayerState, state);
        if (!zox_valid(canvas->value)) {
            zox_logw("Canvas is missing from Player");
            continue;
        }
        if (state->value == zox_player_state_dialogue_begin) {
            state->value = zox_player_state_dialogue_active;
            zox_lerp_float(camera->value, CameraVignette, dialogue_vignette, 2.5f);
            zox_lerp_float(camera->value, CameraBlur, dialogue_blur, 2.5f);
            if (local_mouse) {
                zox_setv(local_mouse, MouseLock, 0);
            }
            zox_add(character->value, DisableMovement);
        } else if (state->value == zox_player_state_dialogue_end) {
            state->value = zox_player_state_play_trigger; // zox_player_state_playing;
            zox_setv(e, PlayerStateDirty, 1);
            // lerp our component value over time to new value
            // trigger_canvas_half_fade(world, canvas->value, pause_fade_time, pause_fade_alpha, 0);
            if (local_mouse) {
                zox_setv(local_mouse, MouseLock, 1);
            }
            zox_remove(character->value, DisableMovement);
        }
    }
} zox_sys_end(PlayerDialogueSystem);

// NOTE: This handles player dialogue state changes
zox_sys2(CameraPlayerStateSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CameraLink);
    zox_sys_in(PlayerStateDirty);
    zox_sys_out(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CameraLink, camera);
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_o(PlayerState, state);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (state->value == zox_player_state_pause_begin) {
            zox_lerp_float(camera->value, CameraVignette, pause_vignette, 0.5f);
        } else if (state->value == zox_player_state_main_menu_begin) {
            state->value = zox_player_state_main_menu;
            zox_lerp_float(camera->value, CameraVignette, main_menu_vignette, 2.5f);
            zox_lerp_float(camera->value, CameraBlur, main_menu_blur, 2.5f);
        } else if (state->value == zox_player_state_play_begin) {
            zox_lerp_float(camera->value, CameraVignette, game_vignette, 2.5f);
            zox_lerp_float(camera->value, CameraBlur, game_blur, 2.5f);
        }
    }
} zox_sys_end(CameraPlayerStateSystem);

