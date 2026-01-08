entity3 spawn_frame(ecs *world, const SpawnFrame data) {
    // Sizing
    // byte font_size = (font_size_frame_label / 4) * ui_scale;
    byte font_size = ui_scale * 3;
    byte font_thickness = ui_scale;
    byte font_outline_thickness = ui_scale <= 2 ? 2 : ui_scale / 2;
    byte label_padding = ui_scale;

    // Spawn frame
    zox_instance(data.element.prefab);
    zox_name("frame");
    set_element_spawn_data(
        world,
        e,
        data.canvas,
        data.parent,
        data.element
    );
    if (data.element.render_disabled) {
        zox_set(e, RenderDisabled, { data.element.render_disabled });
    }
    zox_set(e, Color, { data.texture.fill_color });
    zox_set(e, OutlineColor, { data.texture.outline_color });

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

    Children children = (Children) { 0 };

    // Spawn Icon
    const entity icon = spawn_icon(world, &spawnIcon).x;
    add_to_Children(&children, icon);

    // Spawn Label
    entity zext = 0;
    if (zox_has(data.element.prefab, LabelPrefabLink)) {
        zox_geter_value(data.element.prefab, LabelPrefabLink, entity, prefab_frame_label);
        SpawnZext spawnZext = {
            .canvas = canvas_data,
            .parent = parent_data,
            .zext = {
                .font_fill_color = font_fill_frame_label,
                .font_outline_color = font_outline_frame_label,
                .font_resolution = font_size,
                .font_size = font_size,
                .font_thickness = font_thickness,
                .font_outline_thickness = font_outline_thickness,
            },
            .element = {
                .prefab = prefab_frame_label,
                .layer = data.element.layer + 2,
                .render_disabled = data.element.render_disabled,
                .size = spawnIcon.element.size,
                .anchor = (float2) { 0.5f, 0 },
                .position = (int2) {
                    0, // -(font_size - label_padding),
                    (font_size - label_padding) },
            },
        };
        zext = spawn_zext(world, &spawnZext);
        add_to_Children(&children, zext);
        zox_set_unique_name(zext, "icon_text");
    }

    zox_set_ptr(e, Children, children);

    return (entity3) { e, icon, zext };
}