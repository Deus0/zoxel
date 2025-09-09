// called from game state changes
entity spawn_in_game_ui(
    ecs *world,
    const entity player
) {
    if (!zox_has(player, DeviceMode) || !zox_has(player, CanvasLink)) {
        zox_log("! invalid player in [spawn_in_game_ui]\n")
        return 0;
    }
    const byte device_mode = zox_get_value(player, DeviceMode);
    const entity canvas = zox_get_value(player, CanvasLink);
    byte is_touch = device_mode == zox_device_mode_touchscreen;
#ifdef zoxel_mouse_emulate_touch
    is_touch = 1;
#endif
    zox_geter(player, CharacterLink, characterLink);
    entity e = spawn_menu_game(
        world,
        prefab_menu_game,
        player,
        characterLink->value
    );
    if (is_touch) {
        spawn_in_game_ui_touch(world, player, canvas);
    }
    return e;
}

void spawn_player_game_ui(
    ecs *world,
    const entity player
) {
    spawn_in_game_ui(world, player);
#ifdef zoxm_actions_ui
    const entity canvas = zox_get_value(player, CanvasLink);
    find_child_with_tag(canvas, MenuActions, menu_actions);
    if (!menu_actions) {
        spawn_player_menu_actions(world, player);
    }
#endif
}

void resume_player_delayed(
    ecs *world,
    const entity player
) {
    const entity camera = zox_get_value(player, CameraLink)
    const byte can_roam = zox_get_value(camera, CanRoam)
    if (can_roam == 0 || can_roam == 2) {
        if (local_mouse) {
            zox_set(local_mouse, MouseLock, { 1 })
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
    zox_geter_value(player, CanvasLink, entity, canvas);
    find_child_with_tag(canvas, MenuPaused, menu_paused);
    if (menu_paused) {
        zox_delete(menu_paused)
    }
    find_child_with_tag(canvas, Taskbar, taskbar)
    if (taskbar) {
        zox_delete(taskbar)
    }
    disable_inputs_until_release(
        world,
        player,
        zox_device_mode_none,
        1
    );
    trigger_canvas_half_fade(
        world,
        canvas,
        pause_fade_time,
        pause_fade_alpha,
        0
    );
    const ecs_entity_t pause_event = delay_event(
        world,
        &resume_player_delayed,
        player,
        pause_fade_time
    );
    zox_geter_value(player, PlayerPauseEvent, entity, previous_event);
    if (zox_valid(previous_event)) {
        zox_delete(previous_event)
    }
    zox_set(player, PlayerPauseEvent, { pause_event })
}