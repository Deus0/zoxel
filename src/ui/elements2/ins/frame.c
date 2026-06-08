entity3 spawn_frame(ecs *world, entity prefab, entity prefab_icon, entity prefab_label, entity parent, int2 position, int2 size, int2 icon_size, byte font_size, byte index) {
    color fill = frame_fill;
    color outline = frame_outline;
    entity e = spawn_uic(world, prefab, parent, float2_half, position, size, size, fill, outline);
    zox_name("frame");
    zox_set(e, ElementFillColor, { outline });
    entity icon = spawn_icon(world, prefab_icon, e, int2_zero, icon_size, fill, outline, index).x;
    entity text = 0;
    if (zox_valid(prefab_label)) {
        // byte font_size = ui_scale * 3;
        byte2 label_padding = (byte2) { ui_scale * 2, ui_scale * 1 };
        text = spawn_text(world, prefab_label, e, int2_zero, (float2) { 1, 0 }, font_size, zox_alignment_bottom_right, label_padding, "", color_white, color_black);
        zox_set_unique_name(text, "icon_text");
    }
    return (entity3) { e, icon, text };
}
