entity spawn_menu_paused(
    ecs *world,
    const entity player
) {
    // getters
    // zox_geter_value(player, CanvasLink, entity, canvas)
    // settings
    const byte list_font_size = 32;
    const byte header_font_size = 42;
    // const float2 anchor = (float2) { 0.0f, 1.0f };
    // const int2 position = (int2) { 200, -200 };
    // const byte layer = pause_ui_overlay_layer + 3;
    // const byte is_close_button = 0;

    int elements_count = 0;
    SpawnListElement elements[2];
    elements[elements_count++] = (SpawnListElement) {
        .text = pause_label_confirm,
        .on_click = { &button_event_return_to_game },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = pause_label_exit,
        .on_click = { &button_event_end_game },
    };

    const entity e = spawn_window_list(
        world,
        player,
        menu_paused_header_label,
        header_font_size,
        elements,
        elements_count,
        elements_count,
        list_font_size
    );

    zox_add_tag(e, MenuPaused);
    zox_name("menu_paused");
    return e;
}