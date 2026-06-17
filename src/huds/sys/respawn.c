// When player state changes, for player UI
extern entity zox_get_id_statbars(ecs*);

zox_sys2(PlayerRespawnUISystem) {
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
            zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
            continue;
        }
        if (state->value == zox_player_state_respawn_begin) {
            // TODO: Scan all uis of canvas, if menugame we should delete them
            if (dbg_log) {
                zox_log("[RespawnBegin] Begin: Disposing Game UIs");
            }
            // Fade out on death
            trigger_canvas_half_fade(world, canvas->value, respawn_fade_in_time, respawn_fade_alpha, 1);
            zox_set(camera->value, CameraBlur, { death_blur });
            spawn_respawn_ui(world, e, canvas->value);
            // Incase it was paused
            entity pause_menu = zox_get_child_by_id(world, canvas->value, zox_id(MenuPaused));
            if (zox_valid(pause_menu)) {
                zox_delete(pause_menu);
            }
            entity taskbar = zox_get_child_by_id(world, canvas->value, zox_id(Taskbar));
            if (zox_valid(taskbar)) {
                zox_delete(taskbar);
            }
            /*entity menu = zox_get_child_by_id(world, canvas->value, zox_id(MenuPlay));
            if (zox_valid(menu)) {
                zox_delete(menu)
            }*/
            /*entity statbars = zox_get_child_by_id(world, canvas->value, zox_get_id_statbars(world)); // zox_id(StatBars));
            if (zox_valid(statbars)) {
                zox_delete(statbars);
            }*/
            /*entity crosshair = zox_get_child_by_id(world, canvas->value, zox_id(Crosshair));
            if (zox_valid(crosshair)) {
                zox_delete(crosshair);
            }*/
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
} zox_sys_end(PlayerRespawnUISystem);
