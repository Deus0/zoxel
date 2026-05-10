entity3 spawn_frame(ecs *world, SpawnFrame data) {
    data.texture.fill_color = color_white;
    data.texture.outline_color = color_black;
    byte font_size = ui_scale * frame_label_font_size;
    byte font_padding = ui_scale * frame_label_padding;
    byte font_thickness = ui_scale * 1;
    byte fonto_thickness = ui_scale * 1;
    // Spawn frame
    entity e = spawn_uic(world, data.element.prefab, data.parent.e, float2_half, data.element.position, data.element.size, data.element.size, data.texture.fill_color, data.texture.outline_color);
    // zox_instance(data.element.prefab);
    zox_name("frame");
    zox_set(e, ElementColor, { data.texture.outline_color });
    // Spawn Icon
    int2 size = int2_single(data.icon.size);
    entity icon = spawn_icon(world, data.icon.prefab, e, int2_zero, size , data.icon.texture.fill_color, data.icon.texture.outline_color, data.icon.index).x;
    // Spawn Label
    entity text = 0;
    if (zox_has(data.element.prefab, LabelPrefabLink)) {
        zox_geter_value(data.element.prefab, LabelPrefabLink, entity, prefab_frame_label);
        LayoutParentData canvas_data = data.canvas;
        LayoutParentData parent_data = { .e = e };
        SpawnZext spawnZext = {
            .canvas = canvas_data,
            .parent = parent_data,
            .element = {
                .prefab = prefab_frame_label,
                .layer = data.element.layer + 2,
                .render_disabled = data.element.render_disabled,
                .size = size,
                .anchor = (float2) { 1, 0 },
            },
            .zext = {
                .alignment = zox_alignment_bottom_right,
                .margins = byte2_single(font_padding),
                .font_size = font_size,
                .font_resolution = font_size,
                .font_thickness = font_thickness,
                .font_outline_thickness = fonto_thickness,
                .font_fill_color = color_white,
                .font_outline_color = color_black,
            },
        };
        text = spawn_text(world, spawnZext);
        zox_set_unique_name(text, "icon_text");
        zox_set_parent(world, text, e);
    }
    return (entity3) { e, icon, text };
}
