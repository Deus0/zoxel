entity spawn_menu_load(
    ecs *world,
    const entity player
) {
    // more data
    const char* header_label = "Load";
    const int header_font_size = 72;
    const byte visible_count = 4; // 12;
    const byte layer = 1;
    const byte list_font_size = 32;

    SpawnListElement elements[max_settings + 1];
    int elements_count = 0;

    // TODO: Load folder names here
    elements[elements_count++] = (SpawnListElement) {
        .text = "Continue",
        .on_click = { &button_event_load_confirm },
    };

    elements[elements_count++] = (SpawnListElement) {
        .text = "Return",
        .on_click = { &button_event_load_cancel },
    };

    const entity e = spawn_window_list(
        world,
        player,
        header_label,
        header_font_size,
        elements,
        elements_count,
        visible_count,
        list_font_size
    );
    zox_add_tag(e, MenuLoad);
    zox_name("menu_load");

    return e;
}