// TODO: Just use generic event for TooltipText and add static texts on these buttons

/*byte tooltip_event_main_menu(ecs *world, const TooltipEventData *data) {
    set_tooltip_text(world, data->ui, data->tooltip, "hello world");
    return 1;
}

byte tooltip_event_zoxel_header(ecs* world, const TooltipEventData *data) {
    set_tooltip_text(world, data->ui, data->tooltip, "v0.0.1");
    return 1;
}*/

// extern from events
extern void button_event_new_game(ecs*, ClickEventData);

// List Menus adjust to the menu size
entity spawn_main_menu(ecs *world, entity player, const char* base_header) {
    byte window_alignment = zox_huds_window_alignment;
    float2 window_anchor = zox_huds_window_anchor;
    byte header_font_size = 32 * ui_scale;
    byte list_font_size = zox_huds_element_font_size * ui_scale;
    byte2 padding = byte2_single(8 * ui_scale);
    char header_text[128];
    {
        strncpy(header_text, base_header, sizeof(base_header) - 1);
        header_text[sizeof(base_header) - 1] = '\0';
        header_text[0] = ascii_to_upper(base_header[0]);
    }
    char *label_continue;     // "old blood";
    char *label_new;            // "fresh meat / Wander
    char *label_options;
    char *label_exit;
    if (rand() % 100 >= 90) {
        label_continue = "Old blood";     // "old blood";
        label_new = "Fresh meat";            // "fresh meat / Wander
        label_options = "Beep boops";
        label_exit = "Escape..";
    } else {
        label_continue = "Continue";     // "old blood";
        label_new = "New Game";            // "fresh meat / Wander
        label_options = "Options";
        label_exit = "Exit";
    }
    int elements_count = 0;
    SpawnListElement elements[4];
    byte can_load = has_save_game_directory(game_name);
    byte can_exit = 1;
#ifdef zox_android
    can_exit = 0;
#endif
    if (can_load) {
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
    if (can_exit) {
        elements[elements_count++] = (SpawnListElement) {
            .text = label_exit,
            .on_click = { &button_event_exit_app },
        };
    }
    ClickEvent close_event = { NULL };
    entity spawned[elements_count];
    entity3 e3 = spawn_window_list(
        world,
        prefab_window,
        player,
        header_text,
        header_font_size,
        list_font_size,
        close_event,
        0,
        0,
        0,
        window_alignment,
        window_anchor,
        padding,
        spawned,
        elements,
        elements_count,
        elements_count);
    entity e = e3.x;
    zox_set_unique_name(e, "main_menu");
    zox_add(e, MenuMain);
    zox_add(e, NavigationWindow);
    entity header = e3.z;
    zox_add_tooltip_text(world, header, "v0.0.1");
    // zox_set(e3.z, TooltipEvent, { &tooltip_event_zoxel_header });
    // NOTE: For some reason it was throwing errors if i didnt check the outputs here? for entities....
    int j = 0;
    if (can_load) {
        entity load_button = spawned[j++];
        zox_add_tooltip_text(world, load_button, "Load Game");
    }
    entity b2 = spawned[j++];
    zox_add_tooltip_text(world, b2, "New Game");
    entity b3 = spawned[j++];
    zox_add_tooltip_text(world, b3, "Options");
    if (can_exit) {
        entity exit_button = spawned[j++];
        zox_add_tooltip_text(world, exit_button, "Exit Game");
    }
    for (int i = 0; i < elements_count; i++) {
        zox_set_unique_name(spawned[i], "main_menu_button");
    }
    return e;
}
