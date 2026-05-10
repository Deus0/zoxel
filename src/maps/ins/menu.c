entity spawn_player_menu_map(ecs* world, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    SpawnWindowUsers data = get_default_spawn_window_users_data(world, prefab_menu_map, character, canvas, canvas_size);
    data.header_zext.text = "Map";
    data.element.prefab = prefab_menu_map;
    SpawnWindow2 window_data = {
        .header_text = "Map",
        .header_font_size = 32,
        .header_padding = (byte2) { 8, 4 },
    };
    LayoutParentData canvas_data = (LayoutParentData) { .e = canvas, .size = canvas_size };
    entity e = spawn_window2(world, canvas_data, canvas_data, data.element, window_data, (ClickEvent) { &on_closed_taskbar_window }, 1, 0).x;
    return e;
}
