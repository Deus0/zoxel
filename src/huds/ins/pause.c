entity spawn_menu_paused(ecs *world, entity player) {
    byte alignment = zox_alignment_top_left; // zox_alignment_centre
    byte header_font_size = 10 * ui_scale;
    byte list_font_size = 12 * ui_scale;
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
    entity e = spawn_window_list(world, prefab_window, player, menu_paused_header_label, header_font_size, list_font_size, (ClickEvent) { &on_closed_taskbar_window }, 1, 0, 0, alignment, float2_top_left, byte2_single(4), NULL, elements, elements_count, elements_count).x;
    zox_name("menu_paused");
    zox_add_tag(e, MenuPaused);
    zox_add_tag(e, NavigationWindow);
    zox_set(e, TaskbarToggleLink, { 0 });
    zox_set(e, InitializeEntity, { zox_dirty_trigger });
    // zox_set(e, Anchor, { 0, 1 });
    entity canvas = zox_getv(player, CanvasLink);
    link_window_to_taskbar(world, e, canvas, zox_id(MenuPaused));
    return e;
}
