entity spawn_menu_terminal(ecs *world, entity canvas) {
    if (!zox_valid(canvas) || !zox_has(canvas, Canvas)) {
        zox_log_error("Invalid Canvas");
        return 0;
    }
    byte layer = game_overlay_layer + 1; // 1;
    byte font_size = 32;
    byte2 padding = (byte2) { 12, 8 };
    float2 anchor = { 1.0f, 1.0f };
    int2 position = (int2) { -16, -16 };
    return spawn_label(world, prefab_menu_terminal, canvas, position, anchor, padding, "Terminal", font_size, zox_alignment_top_right, layer, button_fill, button_outline, default_label_font_fill_color,  default_label_font_outline_color);
}
