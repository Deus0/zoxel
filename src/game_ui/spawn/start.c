ecs_entity_t spawn_menu_start(
    ecs *world,
    const entity prefab,
    const entity player,
    const entity canvas,
    const char *header_label
) {
    const float2 anchor = (float2) { 0.5f, 0.5f };
    const int2 position = (int2) { 0, 0 };
    const byte header_font_size = 0;
    const byte font_size = 80;
    const byte is_close_button = 0;
    const byte layer = 3;
    const int labels_count = 1;
    const text_group labels[] = { { label_start } };
    const ClickEvent events[] = { { &button_event_menu_start } };
    const entity e = spawn_ui_list(world,
        prefab,
        canvas,
        header_label,
        labels_count,
        labels_count,
        labels,
        events,
        NULL,
        NULL,
        position,
        anchor,
        is_close_button,
        header_font_size,
        font_size,
        layer,
        0,
        player,
        int2_zero
    );
    zox_name("main_start")
    return e;
}