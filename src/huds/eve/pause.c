extern void delayed_open_game_menu(ecs*, entity);
// When player state changes, for player UI
//  - paused: the game paused shows its pause menu (Start, Escape); a player's own game menus (Y, R) its taskbar

void delayed_spawn_menu_paused(ecs* world, entity e) {
    if (zox_valid(e)) {
        spawn_menu_paused(world, e);
    }
}

void player_state_pause_ui(ecs* world, entity player, byte state) {
    byte dbg_log = 0;
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity camera = zox_get_link(world, player, CameraLink);
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
        // the touch buttons go while the game's paused; in its own menus their menus button closes them
        const entity pausing_game = zox_get_link(world, player, GameLink);
        if (zox_valid(pausing_game) && (zox_getv(pausing_game, GameState) == zox_game_state_paused ||
            zox_getv(pausing_game, GameStateTarget) == zox_game_state_paused)) {
            dispose_menu_game_touch(world, player);
        }
        if (zox_has(camera, CameraBlur)) {
            zox_set(camera, CameraBlur, { pause_blur });
        }
        trigger_canvas_half_fade(
            world,
            canvas,
            pause_fade_time,
            pause_fade_alpha,
            1);
    } else if (state == zox_player_state_paused) {
        // the game paused (Start, Escape): its pause menu; else the player's game menus (Y, R), the game going on
        const entity game = zox_get_link(world, player, GameLink);
        if (zox_valid(game) && zox_getv(game, GameState) == zox_game_state_paused) {
            if (!zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuPaused)))) {
                delay_event(world, &delayed_spawn_menu_paused, player, 0.01f);
            }
        } else if (game_ui_has_taskbar && !zox_valid(zox_get_child_by_id(world, canvas, zox_id(Taskbar)))) {
            spawn_taskbar(world, canvas);
            // the first game menu open for a gamepad (or a headset's hands) to select in; a mouse or touch clicks
            const byte mode = zox_has(player, DeviceMode) ? zox_getv(player, DeviceMode) : 0;
            if (mode != zox_device_mode_keyboardmouse && mode != zox_device_mode_touchscreen) {
                delay_event(world, &delayed_open_game_menu, player, 0.01f);
            }
        }
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
        if (zox_has(camera, CameraBlur)) {
            zox_set(camera, CameraBlur, { 0 });
        }
    }
}
