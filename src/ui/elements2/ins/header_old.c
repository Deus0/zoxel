entity spawn_header(
    ecs *world,
    const entity parent,
    const entity canvas,
    const int2 pixel_position,
    const int2 pixel_size,
    const float2 anchor,
    const char* label,
    const int font_size,
    int header_margins,
    const byte layer,
    const int2 parent_pixel_positionv,
    const int2 parent_pixel_size,
    const byte is_close_button,
    const int2 canvas_size
) {
    const int string_length = strlen(label);
    int2 zext_position = (int2) {
        ((font_size * string_length) / 2) + header_margins / 2,
        0
    };
    float2 zext_anchor = (float2) { 0, 0.5f };
    if (!is_close_button) {
        zext_anchor.x = 0.5f;
        zext_position.x = 0;
    }
    const byte2 padding = (byte2) {
        (int) (font_size * 0.3f),
        (int) (font_size * 0.3f)
    };

    const byte zext_layer = layer + 1;
    const byte button_layer = layer + 2;
    zox_instance(prefab_header)
    zox_name("header")
    zox_set(e, DraggedLink, { parent })
    initialize_element(world, e,
        parent,
        canvas,
        pixel_position,
        pixel_size,
        pixel_size,
        anchor,
        layer
    );

    SpawnZext zext_spawn_data = {
        .canvas = {
            .e = canvas,
            // .size = canvas_size
        },
        .parent = {
            .e = e,
            // .size = pixel_size
        },
        .element = {
            .prefab = prefab_zext,
            .layer = zext_layer,
            .anchor = zext_anchor,
            .position = zext_position
        },
        .zext = {
            .text = label,
            .font_size = font_size,
            .font_resolution = header_font_resolution,
            .font_thickness = header_font_thickness_fill,
            .font_outline_thickness = header_font_thickness_outline,
            .font_fill_color = header_font_fill,
            .font_outline_color = header_font_outline,
            .margins = padding,
        }
    };
    Children children = (Children) { 0, NULL };

    const entity text = spawn_zext(world, &zext_spawn_data);
    add_to_Children(&children, text);

    if (is_close_button) {
        int2 close_button_position = (int2) {
            - (font_size / 2) - padding.x,
            0
        };
        entity b = spawn_close_button(
            world,
            e,
            canvas,
            close_button_position,
            font_size,
            padding,
            button_layer,
            (ClickEvent) { NULL }
        );
        add_to_Children(&children, b);
    }

    zox_set_ptr(e, Children, children);
    return e;
}

entity spawn_header2(ecs *world, SpawnHeader *data) {
    const int string_length = strlen(data->zext.text);
    int2 zext_position = (int2) {
        ((data->zext.font_size * string_length) / 2)
        + data->zext.margins.y,
        0
    };
    float2 zext_anchor = (float2) { 0, 0.5f };
    if (!data->header.is_close_button) {
        zext_anchor.x = 0.5f;
        zext_position.x = 0;
    }
    const byte2 padding = (byte2) { (int) (data->zext.font_size * 0.3f), (int) (data->zext.font_size * 0.3f) };

    const byte zext_layer = data->element.layer + 1;
    const byte button_layer = data->element.layer + 2;
    zox_instance(data->element.prefab)
    zox_name("header")
    zox_set(e, DraggedLink, { data->parent.e })
    initialize_element(
        world,
        e,
        data->parent.e,
        data->canvas.e,
        data->element.position,
        data->element.size,
        data->element.size,
        data->element.anchor,
        data->element.layer
    );
    SpawnZext zextSpawnData = {
        .canvas = data->canvas,
        .parent = {
            .e = e,
            .size = data->element.size
        },
        .element = {
            .prefab = data->header.prefab_zext,
            .layer = zext_layer,
            .anchor = zext_anchor,
            .position = zext_position
        },
        .zext = data->zext
    };
    Children children = (Children) { 0 };
    const entity header_zext = spawn_zext(world, &zextSpawnData);
    add_to_Children(&children, header_zext);
    if (data->header.is_close_button) {
        const int2 close_button_position = (int2) {
            - (data->zext.font_size / 2)
            - data->zext.margins.y,
            0
        };
        entity b = spawn_close_button(
            world,
            e,
            data->canvas.e,
            close_button_position,
            data->zext.font_size,
            padding,
            button_layer,
            (ClickEvent) { NULL }
        );
        add_to_Children(&children, b);
    }
    zox_set_ptr(e, Children, children);
    return e;
}
