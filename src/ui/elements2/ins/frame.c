entity3 spawn_frame(ecs *world, entity prefab, entity parent, int2 position, int2 size, entity prefab_icon, int2 icon_size, byte index) {
    color fill = color_white;
    color outline = color_black;
    byte font_size = ui_scale * frame_label_font_size;
    byte font_padding = ui_scale * frame_label_padding;
    // entity prefab_icon = prefab_element_textured;
    entity e = spawn_uic(world, prefab, parent, float2_half, position, size, size, fill, outline);
    zox_name("frame");
    zox_set(e, ElementColor, { outline });
    // Spawn Icon
    entity icon = spawn_icon(world, prefab_icon, e, int2_zero, icon_size, fill, outline, index).x;
    // Spawn Label
    entity text = 0;
    if (zox_has(prefab, LabelPrefabLink))
    {
        zox_geter_value(prefab, LabelPrefabLink, entity, prefab_text);
        text = spawn_text_new(world, prefab_text, e, int2_zero, (float2) { 1, 0 }, font_size, zox_alignment_bottom_right, byte2_single(font_padding), "", color_white, color_black);
        zox_set_unique_name(text, "icon_text");
    }
    return (entity3) { e, icon, text };
}
