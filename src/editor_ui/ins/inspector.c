entity spawn_inspector(
    ecs *world,
    const entity canvas
) {
    const entity player = 0; // zox_players[0];
    const byte layer = 12;
    const int font_size = 12;
    const int header_font_size = 20;
    const float2 anchor = (float2) { 1, 0.5f };
    const int2 pixel_position = int2_zero;
    const byte is_scrollbar = 1;
    const byte is_close_button = 1;
    const int labels_count = 1;

    int2 window_size = (int2) { 480, 600 };
    const int visible_count = window_size.y / (font_size + 4);

    const entity e = spawn_ui_list(
        world,
        prefab_inspector,
        canvas,
        "inspector",
        labels_count,
        visible_count,
        NULL,
        NULL,
        NULL,
        NULL,
        pixel_position,
        anchor,
        is_close_button,
        header_font_size,
        font_size,
        layer,
        is_scrollbar,
        player,
        window_size
    );
    zox_name("inspector");
    inspector = e;
    return e;
}