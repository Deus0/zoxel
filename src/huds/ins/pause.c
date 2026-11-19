const char *menu_options_header_label = "Ponder";
const char *menu_paused_header_label = "Still";
const char* pause_label_confirm = "Wander";
const char* pause_label_exit = "Leave";

// the pause menu, on a canvas (the shared one over the whole window: one for every player, huds/fun/pause_canvas.c)
entity spawn_menu_paused(ecs *world, entity canvas) {
    byte window_alignment = zox_huds_window_alignment;
    float2 window_anchor = zox_huds_window_anchor;
    byte header_font_size = zox_huds_element_font_size * ui_scale;
    byte list_font_size = zox_huds_element_font_size * ui_scale;
    byte2 list_padding = byte2_single(zox_huds_list_padding * ui_scale);
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
    entity e;
    e = spawn_window_list_on(
        world,
        prefab_window,
        canvas,
        menu_paused_header_label,
        header_font_size,
        list_font_size,
        (ClickEvent) {
            &button_event_return_to_game
        },
        1,
        0,
        0,
        window_alignment,
        window_anchor,
        list_padding,
        NULL,
        elements,
        elements_count,
        elements_count).x;
    zox_name("menu_paused");
    zox_add(e, MenuPaused);
    zox_add(e, NavigationWindow);
    zox_add(e, PreInitialize);
    return e;
}
