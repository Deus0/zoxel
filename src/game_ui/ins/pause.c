entity spawn_menu_paused(ecs *world, const entity player) {
    // settings
    byte header_font_size = 10 * ui_scale;
    byte list_font_size = 8 * ui_scale;

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
        list_font_size,
        (ClickEvent) { &on_closed_taskbar_window },
        1
    );

    zox_add_tag(e, MenuPaused);
    zox_name("menu_paused");
    return e;
}