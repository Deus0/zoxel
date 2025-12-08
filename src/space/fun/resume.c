

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
    // TODO: This should be done when changing player states
    disable_inputs_until_release(
        world,
        player,
        zox_device_mode_none,
        1
    );

    zox_geter_value(player, CanvasLink, entity, canvas);
    find_child_with_tag(canvas, MenuPaused, menu_paused);
    if (zox_valid(menu_paused)) {
        zox_delete(menu_paused);
    }
    find_child_with_tag(canvas, Taskbar, taskbar);
    if (zox_valid(taskbar)) {
        zox_delete(taskbar);
    }
    trigger_canvas_half_fade(
        world,
        canvas,
        pause_fade_time,
        pause_fade_alpha,
        0
    );
    const entity pause_event = delay_event(
        world,
        &resume_player_delayed,
        player,
        pause_fade_time
    );
    zox_geter_value(player, PlayerPauseEvent, entity, previous_event);
    if (zox_valid(previous_event)) {
        zox_delete(previous_event);
    }
    // zox_set(player, PlayerPauseEvent, { pause_event });
}