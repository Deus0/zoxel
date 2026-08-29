// When player state changes, for player UI

void delayed_spawn_menu_paused(ecs* world, entity e) {
    if (zox_valid(e)) {
        spawn_menu_paused(world, e);
    }
}

void player_state_pause_ui(ecs* world, entity player, byte state) {
    byte dbg_log = 0;
    entity canvas = zox_getv(player, CanvasLink);
    entity camera = zox_getv(player, CameraLink);
    if (!zox_valid(canvas)) {
        zox_loge("Canvas is missing for Player [%s]", zox_getn(player));
        return;
    }
    if (!zox_valid(camera)) {
        zox_loge("Camera is missing for Player [%s]", zox_getn(player));
        return;
    }
    if (state == zox_player_state_pause_begin) {
        if (dbg_log) {
            zox_log("[%s] Begin: Disposing Game UIs", state == zox_player_state_pause_begin ? "PauseBegin" : "RespawnBegin");
        }
        dispose_menu_game_touch(world, player);
        zox_set(camera, CameraBlur, { pause_blur });
        trigger_canvas_half_fade(
            world,
            canvas,
            pause_fade_time,
            pause_fade_alpha,
            1);
    } else if (state == zox_player_state_paused) {
        if (game_ui_has_taskbar) {
            spawn_taskbar(world, canvas);
        }
        delay_event(
            world,
            &delayed_spawn_menu_paused,
            player,
            0.01f);
    } else if (state == zox_player_state_resume_begin) {
        entity pause_menu = zox_get_child_by_id(
            world,
            canvas,
            zox_id(MenuPaused));
        if (zox_valid(pause_menu)) {
            zox_delete(pause_menu);
        }
        entity taskbar = zox_get_child_by_id(
            world,
            canvas,
            zox_id(Taskbar));
        if (zox_valid(taskbar)) {
            zox_delete(taskbar);
        }
        trigger_canvas_half_fade(world, canvas, pause_fade_time, pause_fade_alpha, 0);
    } else if (state == zox_player_state_play_begin) {
        zox_set(camera, CameraBlur, { 0 });
    }
}

/*zox_sys2(PlayerPauseUISystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CanvasLink);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(CameraLink, camera);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(canvas->value)) {
            zox_loge("Canvas is missing for Player [%s]", zox_getn(e));
            continue;
        }
        if (!zox_valid(camera->value)) {
            zox_loge("Camera is missing for Player [%s]", zox_getn(e));
            continue;
        }
        if (state->value == zox_player_state_pause_begin) {
            if (dbg_log) {
                zox_log("[%s] Begin: Disposing Game UIs", state->value == zox_player_state_pause_begin ? "PauseBegin" : "RespawnBegin");
            }
            dispose_menu_game_touch(world, e);
            zox_set(camera->value, CameraBlur, { pause_blur });
            trigger_canvas_half_fade(world, canvas->value, pause_fade_time, pause_fade_alpha, 1);
        } else if (state->value == zox_player_state_paused) {
            if (game_ui_has_taskbar) {
                spawn_taskbar(world, canvas->value);
            }
            delay_event(world, &delayed_spawn_menu_paused, e, 0.01f);
        } else if (state->value == zox_player_state_resume_begin) {
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
        }
    }
} zox_sys_end(PlayerPauseUISystem);*/
