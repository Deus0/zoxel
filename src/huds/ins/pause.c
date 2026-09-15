const char *menu_options_header_label = "Ponder";
const char *menu_paused_header_label = "Still";
const char* pause_label_confirm = "Wander";
const char* pause_label_exit = "Leave";

entity spawn_menu_paused(ecs *world, entity player) {
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
    entity e = spawn_window_list(
        world,
        prefab_window,
        player,
        menu_paused_header_label,
        header_font_size,
        list_font_size,
        (ClickEvent) {
            &on_closed_taskbar_window
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
    entity canvas = zox_get_link(world, player, CanvasLink);
    link_window_to_taskbar(
        world,
        e,
        canvas,
        zox_id(MenuPaused));
    zox_add(e, PreInitialize);
    return e;
}
