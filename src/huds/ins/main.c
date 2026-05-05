byte tooltip_event_main_menu(ecs *world, const TooltipEventData *data) {
    set_entity_text(world, data->tooltip, "hello world");
    return 1;
}

byte tooltip_event_zoxel_header(ecs* world, const TooltipEventData *data) {
    set_entity_text(world, data->tooltip, "zoxel v0.1.1");
    return 1;
}

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

    entity elements2[elements_count];
    entity3 e3 = spawn_window_list(world, prefab_window, player, header_label, header_font_size, elements, elements_count, elements_count, list_font_size, (ClickEvent) { NULL }, 0, zox_window_main_menu, 0, zox_alignment_centre, byte2_single(4), elements2);
    zox_set_unique_name(e3.x, "main_menu");
    zox_add_tag(e3.x, MenuMain);
    zox_add_tag(e3.x, NavigationWindow);

    zox_set(e3.z, TooltipEvent, { &tooltip_event_zoxel_header });
    // zox_set(elements2[0], TooltipEvent, { &tooltip_event_main_menu });

    return e3.x;
}
