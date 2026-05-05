entity3 spawn_frame(ecs *world, SpawnFrame data) {
    data.texture.fill_color = color_white;
    data.texture.outline_color = color_black;
    byte font_size = ui_scale * frame_label_font_size;
    byte font_padding = ui_scale * frame_label_padding;
    byte font_thickness = ui_scale * 1;
    byte fonto_thickness = ui_scale * 1;

    // Spawn frame
    zox_instance(data.element.prefab);
    zox_name("frame");
    set_element_spawn_data(world, e, data.canvas, data.parent, data.element);
    zox_set(e, RenderDisabled, { data.element.render_disabled });
    zox_set(e, Color, { data.texture.fill_color });
    zox_set(e, OutlineColor, { data.texture.outline_color });
    zox_set(e, ElementColor, { data.texture.outline_color });

    LayoutParentData canvas_data = data.canvas;
    LayoutParentData parent_data = {
        .e = e,
        .position = data.element.position_in_canvas,
        .size = data.element.size
    };

    SpawnIcon spawnIcon = {
        .canvas = canvas_data,
        .parent = parent_data,
        .element = {
            .prefab = data.icon.prefab,
            .layer = data.element.layer + 1,
            .size = int2_single(data.icon.size),
            .anchor = float2_half,
        },
        .texture = data.icon.texture,
        .index = data.icon.index,
        .texture_size = data.icon.texture_size
    };

    // Children children = (Children) { 0 };

    // Spawn Icon
    entity icon = spawn_icon(world, &spawnIcon).x;
    // add_to_Children(&children, icon);
    zox_set_parent(world, icon, e);

    // Spawn Label
    entity text = 0;

    if (zox_has(data.element.prefab, LabelPrefabLink)) {
        zox_geter_value(data.element.prefab, LabelPrefabLink, entity, prefab_frame_label);
        SpawnZext spawnZext = {
            .canvas = canvas_data,
            .parent = parent_data,
            .element = {
                .prefab = prefab_frame_label,
                .layer = data.element.layer + 2,
                .render_disabled = data.element.render_disabled,
                .size = spawnIcon.element.size,
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

        text = spawn_zext(world, spawnZext);
        zox_set_unique_name(text, "icon_text");

        // add_to_Children(&children, text);
        zox_set_parent(world, text, e);
    }

    // zox_set_ptr(e, Children, children);

    return (entity3) { e, icon, text };
}
