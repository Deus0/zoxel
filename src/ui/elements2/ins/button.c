entity spawn_button(ecs *world, LayoutParentData canvas_data, LayoutParentData parent, ElementSpawnData element_data, SpawnTextData zext_data, SpawnButtonData button_data) {
    int text_length = zext_data.text != NULL ? strlen(zext_data.text) : 0;
    int2 size = (int2) { zext_data.font_size * text_length, zext_data.font_size };
    element_data.size = (int2) {
        size.x + zext_data.margins.x * 2,
        size.y + zext_data.margins.y * 2
    };
    zox_instance(element_data.prefab);
    zox_name("button");
    zox_set(e, Color, { button_data.fill });
    zox_set(e, OutlineColor, { button_data.outline });
    if (element_data.render_disabled) {
        zox_set(e, RenderDisabled, { element_data.render_disabled });
    }
    set_element_spawn_data(world, e, canvas_data, parent, element_data);
    // text
    SpawnZext zext_data2 = {
        .canvas = canvas_data,
        .zext = zext_data,
        .parent = { .e = e },
        .element = {
            .prefab = button_data.prefab_zext,
            .layer = element_data.layer + 1,
            .anchor = float2_half,
            .size = size,
            .render_disabled = element_data.render_disabled,
        },
    };
    entity text = spawn_text(world, zext_data2);
    zox_set_parent(world, text, e);
    return e;
}
