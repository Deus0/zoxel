// TODO: Refactor these modules onto the game ui
extern entity spawn_menu_game_stats(ecs*, entity, entity);

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
    spawn_menu_game_stats(world, e, player);
    local_menu_game = e;
    // link to character
    zox_muter(character, ElementLinks, elements);
    add_to_ElementLinks(elements, e);
    zox_set(e, ElementHolder, { character });
    return e;
}

void dispose_menu_game(ecs *world, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuPlay));
    if (zox_valid(menu)) {
        zox_delete(menu)
    }
    entity crosshair = zox_get_child_by_id(world, canvas, zox_id(Crosshair));
    if (zox_valid(crosshair)) {
        zox_delete(crosshair)
    }
    dispose_menu_game_touch(world, player);
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
