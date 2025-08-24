// List Menus adjust to the menu size
entity spawn_main_menu(
    ecs *world,
    const entity player,
    const entity canvas,
    const char *header_label
) {
    const byte window_layer = 3;
    const byte header_font_size = 80;
    byte list_font_size = 48;

    // # List #
    SpawnListElement elements[4];
    int elements_count = 0;
    if (has_save_game_directory(game_name)) {
        elements[elements_count++] = (SpawnListElement) {
            .text = label_continue,
            .on_click = { &button_event_continue_game },
        };
    }
    elements[elements_count++] = (SpawnListElement) {
        .text = label_new,
        .on_click = { &button_event_new_game },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = label_options,
        .on_click = { &button_event_menu_options },
    };
#ifndef zox_android
    elements[elements_count++] = (SpawnListElement) {
        .text = label_exit,
        .on_click = { &button_event_exit_app },
    };
#endif

    const entity e = spawn_window_list(
        world,
        player,
        header_label,
        header_font_size,
        elements,
        elements_count,
        elements_count,
        list_font_size
    );
    zox_name("main_menu");
    zox_add_tag(e, MenuMain);
    return e;
}