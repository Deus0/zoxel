entity spawn_fps_display(ecs *world, entity canvas) {
    if (!zox_valid(canvas) || !zox_has(canvas, Canvas)) {
        zox_log_error("Invalid Canvas");
        return 0;
    }

    entity parent = canvas;
    byte layer = game_overlay_layer + 1; // 1;
    byte font_size = 32;
    byte2 padding = (byte2) { 12, 8 };
    float2 anchor = { 1.0f, 1.0f };
    int2 pixel_position = (int2) { -16, -16 };
    int2 parent_pixel_size = zox_get_value(canvas, LayoutSize)
    return spawn_label_background(
        world,
        fps_display_prefab,
        parent,
        canvas,
        pixel_position,
        anchor, padding,
        "",
        font_size,
        zox_alignment_top_right,
        layer,
        int2_half(parent_pixel_size),
        parent_pixel_size,
        button_fill, button_outline,
        default_label_font_fill_color,
        default_label_font_outline_color,
        0
    );
}
