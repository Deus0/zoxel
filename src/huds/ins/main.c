// List Menus adjust to the menu size
entity spawn_main_menu(ecs *world, entity player, const char *header_label) {

    byte header_font_size = 20 * ui_scale;
    byte list_font_size = 12 * ui_scale;

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

    entity e = spawn_window_list(world, prefab_menu_game, player, header_label, header_font_size, elements, elements_count, elements_count, list_font_size, (ClickEvent) { NULL }, 0, zox_window_main_menu, 0);
    zox_name("main_menu");

    zox_add_tag(e, MenuMain);
    zox_add_tag(e, NavigationWindow);

    return e;
}
