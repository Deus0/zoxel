entity spawn_window_inspector(ecs *world, entity canvas, entity player, entity target) {

    byte layer = 12;
    int font_size = 12;
    int header_font_size = 20;
    float2 anchor = (float2) { 1, 0.5f };
    int2 pixel_position = int2_zero;
    byte is_scrollbar = 1;
    byte is_close_button = 1;
    int labels_count = 1;

    int2 window_size = (int2) { 480, 600 };
    int visible_count = window_size.y / (font_size + 4);

    entity e = spawn_ui_list(world, prefab_inspector, canvas, "inspector", labels_count, visible_count, NULL, NULL, NULL, NULL, pixel_position, anchor, is_close_button, header_font_size, font_size, layer, is_scrollbar, player, window_size);
    zox_name("inspector");

    zox_set(e, WindowType, { zox_window_inspector });
    zox_set(e, EditorTarget, { target });

    inspector = e;

    return e;
}
