
void resume_player_delayed(
    ecs *world,
    const entity player
) {
    const entity camera = zox_get_value(player, CameraLink);
    const byte can_roam = zox_get_value(camera, CanRoam);
    if (can_roam == 0 || can_roam == 2) {
        if (local_mouse) {
            zox_set(local_mouse, MouseLock, { 1 });
        }
    }
    // return to regular ui
    const entity character = zox_get_value(player, CharacterLink)
    if (!zox_alive(character)) {
        return;
    }
    if (can_roam == 0) { // not roaming, return character movement
        zox_set(character, DisableMovement, { 0 });
    }
    spawn_in_game_ui(world, player);
}

void resume_player(
    ecs *world,
    const entity player
) {
    // zox_set(player, PlayerPauseEvent, { pause_event });
}

void PlayerUIGameResumeSystem(iter *it) {
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

        // TODO: Add resuming state
        if (state->value != zox_player_state_playing) {
            continue;
        }

        find_child_with_tag(canvas->value, MenuPaused, menu_paused);
        if (zox_valid(menu_paused)) {
            zox_delete(menu_paused);
        }
        find_child_with_tag(canvas->value, Taskbar, taskbar);
        if (zox_valid(taskbar)) {
            zox_delete(taskbar);
        }
        trigger_canvas_half_fade(
            world,
            canvas->value,
            pause_fade_time,
            pause_fade_alpha,
            0
        );
        const entity pause_event = delay_event(
            world,
            &resume_player_delayed,
            e,
            pause_fade_time
        );
        if (zox_valid(pause_event_link->value)) {
            zox_delete(pause_event_link->value)
        }
        pause_event_link->value = pause_event;
    }
} zoxd_system2(PlayerUIGameResumeSystem);