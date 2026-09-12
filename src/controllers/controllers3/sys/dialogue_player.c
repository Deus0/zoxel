// NOTE: This handles player dialogue state changes
zox_sys2(PlayerDialogueSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(PlayerState, state);
        /*entity canvas = zox_get_link(world, e, Canvas);
        if (!zox_valid(canvas)) {
            // zox_logw("Canvas is missing from Player");
            continue;
        }*/
        entity character = zox_get_link(world, e, Character);
        if (state->value == zox_player_state_dialogue_begin) {
            entity camera = zox_get_link(world, e, CameraLink);
            state->value = zox_player_state_dialogue_active;
            zox_lerp_float(
                camera,
                CameraVignette,
                dialogue_vignette,
                2.5f);
            zox_lerp_float(
                camera,
                CameraBlur,
                dialogue_blur,
                2.5f);
            entity mouse = zox_get_child_by_id(
                world,
                e,
                zox_id(Mouse));
            if (mouse) {
                zox_setv(mouse, MouseLock, 0);
            }
            zox_add(character, DisableMovement);
        } else if (state->value == zox_player_state_dialogue_end) {
            state->value = zox_player_state_play_trigger; // zox_player_state_playing;
            zox_setv(e, PlayerStateDirty, 1);
            // lerp our component value over time to new value
            // trigger_canvas_half_fade(world, canvas->value, pause_fade_time, pause_fade_alpha, 0);
            entity mouse = zox_get_child_by_id(world, e, zox_id(Mouse));
            if (mouse) {
                zox_setv(mouse, MouseLock, 1);
            }
            zox_remove(character, DisableMovement);
        }
    }
} zox_sys_end(PlayerDialogueSystem);

// NOTE: This handles player dialogue state changes
zox_sys2(CameraPlayerStateSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_out(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_o(PlayerState, state);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity camera = zox_get_link(world, e, CameraLink);
        if (!zox_valid(camera)) {
            zox_loge("Player's camera is invalid [%s]", zox_sys_e_name);
            continue;
        }
        if (state->value == zox_player_state_pause_begin) {
            zox_lerp_float(camera, CameraVignette, pause_vignette, 0.5f);
        } else if (state->value == zox_player_state_main_menu_begin) {
            state->value = zox_player_state_main_menu;
            zox_lerp_float(camera, CameraVignette, main_menu_vignette, 2.5f);
            zox_lerp_float(camera, CameraBlur, main_menu_blur, 2.5f);
        } else if (state->value == zox_player_state_play_begin) {
            zox_lerp_float(camera, CameraVignette, game_vignette, 2.5f);
            zox_lerp_float(camera, CameraBlur, game_blur, 2.5f);
        }
    }
} zox_sys_end(CameraPlayerStateSystem);

