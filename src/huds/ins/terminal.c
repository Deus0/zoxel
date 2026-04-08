entity spawn_menu_terminal(ecs *world, entity canvas) {
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

    zox_geter_value(canvas, LayoutSize, int2, parent_size);
    return spawn_label_background(
        world,
        prefab_menu_terminal,
        parent,
        canvas,
        pixel_position,
        anchor, padding,
        "Terminal",
        font_size,
        zox_mesh_alignment_top_right,
        layer,
        int2_half(parent_size),
        parent_size,
        button_fill, button_outline,
        default_label_font_fill_color,
        default_label_font_outline_color,
        0
    );
}
