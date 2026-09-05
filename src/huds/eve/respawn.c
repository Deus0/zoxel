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
    entity canvas = zox_get_link(world, player, Canvas);
    entity camera = zox_get_link(world, player, Camera);
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
