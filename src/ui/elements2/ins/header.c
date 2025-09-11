entity spawn_header3(
    ecs *world,
    const LayoutParentData canvas_data,
    const LayoutParentData parent_data,
    const ElementSpawnData element_data,
    const SpawnTextData zext,
    const SpawnHeaderData header,
    ClickEvent on_click
) {
    zox_instance(element_data.prefab);
    zox_name("header");
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
    zox_set(e, DraggedLink, { parent_data.e });
    Children children = (Children) { 0 };

    // # Header Text # - Left Aligned
    const int string_length = strlen(zext.text);
    int2 zext_position = (int2) {
        ((zext.font_size * string_length) / 2) + zext.margins.x,
        0
    };
    float2 zext_anchor = (float2) { 0, 0.5f };
    if (!header.is_close_button) {
        zext_anchor.x = 0.5f;
        zext_position.x = 0;
    }
    SpawnZext zext_data = {
        .canvas = canvas_data,
        .parent = {
            .e = e,
            .size = element_data.size,
        },
        .element = {
            .prefab = header.prefab_zext,
            .layer = element_data.layer + 1,
            .anchor = zext_anchor,
            .position = zext_position
        },
        .zext = zext
    };
    const entity header_zext = spawn_zext(world, &zext_data);
    zox_set_unique_name(header_zext, "header_text");
    add_to_Children(&children, header_zext);

    // # Header Close Button #
    if (header.is_close_button) {
        /*const byte2 padding = (byte2) {
            (int) (zext.font_size * 0.3f),
            (int) (zext.font_size * 0.3f)
        };*/
        const int2 close_button_position = (int2) {
            - (zext.font_size + zext.margins.x * 2) / 2,
            0
        };
        entity b = spawn_close_button(
            world,
            e,
            canvas_data.e,
            close_button_position,
            zext.font_size * 0.6,
            zext.margins, // padding,
            element_data.layer + 2,
            on_click
        );
        add_to_Children(&children, b);
    }

    zox_set_ptr(e, Children, children);

    return e;
}