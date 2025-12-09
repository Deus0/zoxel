void pause_player_ending(ecs *world, const entity player) {
    const entity canvas = zox_get_value(player, CanvasLink)
    if (game_ui_has_taskbar) {
        spawn_taskbar(world, prefab_taskbar, canvas, canvas, pause_ui_overlay_layer + 1);
    }
}

// extern void dispose_menu_game(ecs *world, const entity player);

void PlayerUIGamePauseSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CameraLink);
    zox_sys_in(CanvasLink);
    zox_sys_out(PlayerPauseEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CameraLink, camera);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_o(PlayerPauseEvent, pause_event_link);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (state->value != zox_player_state_paused) {
            continue;
        }

        dispose_menu_game(world, e); // check this, ingame ui should now be linked to player, got from canvas

        // disable_inputs_until_release(world, player, zox_device_mode_none, 1);
        zox_geter_value(camera->value, CanRoam, byte, can_roam);

        if (can_roam == 2) { // if attached to character
            zox_set(camera->value, CanRoam, { 1 });
        } else {
            zox_geter_value(e, CharacterLink, entity, character);
            if (zox_alive(character)) {
                zox_set(character, DisableMovement, { 1 });
            }
        }
        // zox_log("player paused [%s] [%s]\n", zox_get_name(player), zox_get_name(canvas))
        trigger_canvas_half_fade(
            world,
            canvas->value,
            pause_fade_time,
            pause_fade_alpha,
            1
        );
        const entity pause_event = delay_event(
            world,
            &pause_player_ending,
            e,
            pause_fade_time
        );
        if (zox_valid(pause_event_link->value)) {
            zox_delete(pause_event_link->value)
        }
        pause_event_link->value = pause_event;

    }
} zoxd_system2(PlayerUIGamePauseSystem);