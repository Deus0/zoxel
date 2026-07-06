entity spawn_fps_display(ecs *world, entity canvas) {
    if (!zox_valid(canvas) || !zox_has(canvas, Canvas)) {
        zox_log_error("Invalid Canvas");
        return 0;
    }
    byte layer = game_overlay_layer + 1; // 1;
    byte font_size = 32;
    byte2 padding = (byte2) { 12, 8 };
    float2 anchor = { 1.0f, 1.0f };
    int2 position = (int2) { -16, -16 };
    // int2 psize = zox_get_value(canvas, LayoutSize);
    entity e = spawn_label(world, fps_display_prefab, canvas, position, anchor, padding, "", font_size, zox_alignment_top_right, layer, button_fill, button_outline, button_font_fill, button_font_outline);
    zox_set_unique_name(e, "fps_viewer");
    return e;
}
