// When player state changes, for player UI
extern entity zox_get_id_statbars(ecs*);

void player_state_respawn_ui(ecs* world, entity player, byte state) {
    byte dbg_log = 0;
    const double respawn_fade_out_time = 2.5;
    const double respawn_fade_in_time = 1.5;
    const float respawn_fade_alpha = 0.9f;
    const float death_blur = 0.4f;
    byte is_spawn = state == zox_player_state_respawn_begin;
    byte is_destroy = state == zox_player_state_respawn;
    if (!is_destroy && !is_spawn) {
        return;
    }
    if (!zox_has(player, CanvasLink) || !zox_has(player, CameraLink)) {
        zox_loge("[player_state_respawn_ui] Invalid [player]");
        return;
    }
    entity canvas = zox_getv(player, CanvasLink);
    entity camera = zox_getv(player, CameraLink);
    if (!zox_valid(canvas) || !zox_valid(camera)) {
        // zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
        return;
    }
    entity respawn_ui = zox_get_child_by_id(
        world,
        canvas,
        zox_id(RespawnUI));
    if (is_spawn && !zox_valid(respawn_ui)) {
        // Incase it was paused
        // TODO: Move these to their own event functions
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
        entity touch_ui = zox_get_child_by_id(
            world,
            canvas,
            zox_id(MenuPlayTouch));
        if (zox_valid(touch_ui)) {
            zox_delete(touch_ui);
        }
        // Fade out on death
        trigger_canvas_half_fade(
            world,
            canvas,
            respawn_fade_in_time,
            respawn_fade_alpha,
            1);
        zox_setv(camera, CameraBlur, death_blur);
        spawn_respawn_ui(
            world,
            canvas);
        if (dbg_log) {
            zox_log("[RespawnBegin] Begin: Disposing Game UIs");
        }
    } else if (is_destroy && zox_valid(respawn_ui)) {
        if (dbg_log) {
            zox_log("Destroying [RespawnUI] [%s]",
                zox_getn(respawn_ui));
        }
        zox_setv(camera, CameraBlur, 0);
        trigger_canvas_half_fade(
            world,
            canvas,
            respawn_fade_out_time,
            respawn_fade_alpha,
            0);
        zox_delete(respawn_ui);
    }
}

/*zox_sys2(PlayerRespawnUISystem) {
    byte dbg_log = 0;
    double respawn_fade_out_time = 2.5;
    double respawn_fade_in_time = 1.5;
    float respawn_fade_alpha = 0.9f;
    float death_blur = 0.4f;
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
            // zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
            continue;
        }
        if (state->value == zox_player_state_respawn_begin) {
            // TODO: Scan all uis of canvas, if menugame we should delete them
            if (dbg_log) {
                zox_log("[RespawnBegin] Begin: Disposing Game UIs");
            }
            // Fade out on death
            trigger_canvas_half_fade(
                world,
                canvas->value,
                respawn_fade_in_time,
                respawn_fade_alpha,
                1);
            zox_set(camera->value, CameraBlur, { death_blur });
            spawn_respawn_ui(world, canvas->value);
            // Incase it was paused
            entity pause_menu = zox_get_child_by_id(world, canvas->value, zox_id(MenuPaused));
            if (zox_valid(pause_menu)) {
                zox_delete(pause_menu);
            }
            entity taskbar = zox_get_child_by_id(world, canvas->value, zox_id(Taskbar));
            if (zox_valid(taskbar)) {
                zox_delete(taskbar);
            }
            dispose_menu_game_touch(world, e);
        } else if (state->value == zox_player_state_respawn) {
            // When respawn, remove the fade screen
            entity respawn_ui = zox_get_child_by_id(world, canvas->value, zox_id(RespawnUI));
            if (zox_valid(respawn_ui)) {
                zox_loge("RespawnUI Deleted [%s]", zox_get_name(respawn_ui));
                zox_delete(respawn_ui);
            } else {
                zox_loge("RespawnUI not found on canvas");
            }
            zox_set(camera->value, CameraBlur, { 0 });
            trigger_canvas_half_fade(world, canvas->value, respawn_fade_out_time, respawn_fade_alpha, 0);
        }
    }
} zox_sys_end(PlayerRespawnUISystem);*/
