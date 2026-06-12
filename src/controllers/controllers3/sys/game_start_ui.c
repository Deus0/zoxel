entity spawn_menu_game(ecs *world, entity prefab, entity player, entity character) {
    if (!zox_valid(character) || !zox_valid(player)) {
        zox_log_error("[!spawn_menu_game] Issue with character or player, invalid");
        return 0;
    }
    entity canvas = zox_get_value(player, CanvasLink);
    int2 canvas_size = zox_get_value(canvas, LayoutSize);
    // spawn a crosshair first
    int2 csize = int2_single((crosshair_pixel_size.x / 4) * ui_scale);
    spawn_ui(world, prefab_crosshair, canvas, float2_half, int2_zero, csize, csize);
    // make layout2 instead of element_invisible
    entity e = spawn_layout2_on_canvas(world, prefab, canvas, int2_zero, canvas_size, float2_half);
    zox_name("menu_game");
    // link to character
    zox_muter(character, ElementLinks, elements);
    add_to_ElementLinks(elements, e);
    zox_set(e, ElementHolder, { character });
    // spawn_menu_game_stats(world, player);
    return e;
}

// called from game state changes
entity spawn_in_game_ui(ecs *world, entity player) {
    if (!zox_has(player, DeviceMode) || !zox_has(player, CanvasLink)) {
        zox_log_error("Invalid player in [spawn_in_game_ui]")
        return 0;
    }
    zox_geter_value(player, DeviceMode, byte, device_mode);
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter(player, CharacterLink, character);
    entity e = spawn_menu_game(world, prefab_menu_play, player, character->value);
    if (device_mode == zox_device_mode_touchscreen || zox_dbg_touch_with_mouse) {
        spawn_in_game_ui_touch(world, player, canvas);
    }
    return e;
}

// Spawn the UIs
zox_sys2(PlayerBeginUISystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(PlayerStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(PlayerStateDirty, dirty);
        if (state->value == zox_player_state_play_begin && dirty->value == zox_dirty_active) {
            spawn_in_game_ui(world, e);
            spawn_menu_actions(world, e);
        }
    }
} zox_sys_end(PlayerBeginUISystem);
