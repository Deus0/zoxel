entity spawn_button(
    ecs *world,
    const LayoutParentData canvas_data,
    const LayoutParentData parent_data,
    ElementSpawnData element_data,
    const SpawnTextData zext_data,
    const SpawnButtonData button_data
) {
    int text_length = zext_data.text != NULL ? strlen(zext_data.text) : 0;
    const int2 zext_size = (int2) {
        zext_data.font_size * text_length,
        zext_data.font_size
    };
    element_data.size = (int2) {
        zext_size.x + zext_data.margins.x * 2,
        zext_size.y + zext_data.margins.y * 2
    };

    zox_instance(element_data.prefab);
    zox_name("button");
    set_element_spawn_data(
        world,
        e,
        canvas_data,
        parent_data,
        element_data
    );
    if (element_data.render_disabled) {
        zox_set(e, RenderDisabled, { element_data.render_disabled });
    }

    zox_set(e, Color, { button_data.fill });
    zox_set(e, OutlineColor, { button_data.outline });

    Children children = (Children) { 0 };

    // text
    SpawnZext zext_data2 = {
        .canvas = canvas_data,
        .zext = zext_data,
        .parent = { .e = e },
        .element = {
            .prefab = button_data.prefab_zext,
            .layer = element_data.layer + 1,
            .anchor = float2_half,
            .size = zext_size,
            .render_disabled = element_data.render_disabled,
        },
    };
    const entity zext = spawn_zext(world, &zext_data2);
    add_to_Children(&children, zext);

    zox_set_ptr(e, Children, children);
    return e;
}