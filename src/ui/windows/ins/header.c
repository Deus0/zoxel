entity spawn_header3(ecs *world, LayoutParentData canvas_data, LayoutParentData parent_data, ElementSpawnData element_data, SpawnTextData zext, SpawnHeaderData header, ClickEvent on_click) {

    zox_instance(element_data.prefab);
    zox_name("header");

    set_element_spawn_data(world, e, canvas_data, parent_data, element_data);

    zox_set(e, RenderDisabled, { element_data.render_disabled });

    zox_set(e, DraggedLink, { parent_data.e });

    // # Header Text # - Left Aligned
    int string_length = strlen(zext.text);
    int2 zext_position = (int2) {
        ((zext.font_size * string_length) / 2) + zext.margins.x,
        0
    };

    float2 zext_anchor = (float2) { 0, 0.5f };
    if (!header.is_close_button) {
        zext_anchor.x = 0.5f;
        zext_position.x = 0;
    }

    SpawnZext tdata = {
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

    entity text = spawn_text(world, tdata);
    zox_set_unique_name(text, "header_text");
    zox_set_parent(world, text, e);

    // # Header Close Button #
    if (header.is_close_button) {
        entity close_button = spawn_close_button(world, e, canvas_data.e, zext.font_size, zext.margins.y, element_data.layer + 2, on_click);
        zox_set_parent(world, close_button, e);
    }

    return e;
}

entity spawn_header(ecs *world, entity parent, entity canvas, int2 pixel_position, int2 pixel_size, float2 anchor, const char* label, int font_size, int header_margins, byte layer, int2 parent_position, int2 parent_size, byte is_close_button, int2 canvas_size) {

    int string_length = strlen(label);
    int2 zext_position = (int2) {
        ((font_size * string_length) / 2) + header_margins / 2,
        0
    };

    float2 zext_anchor = (float2) { 0, 0.5f };

    if (!is_close_button) {
        zext_anchor.x = 0.5f;
        zext_position.x = 0;
    }

    byte2 padding = (byte2) {
        (int) (font_size * 0.3f),
        (int) (font_size * 0.3f)
    };
    byte font_thickness = header_font_thickness * ui_scale;
    byte fonto_thickness = header_font_thickness * ui_scale;

    byte zext_layer = layer + 1;
    byte button_layer = layer + 2;

    zox_instance(prefab_header);
    zox_name("header");
    zox_set(e, DraggedLink, { parent });
    initialize_element(world, e, parent, canvas, pixel_position, pixel_size, pixel_size, anchor, layer);

    SpawnZext zext_spawn_data = {
        .canvas = { .e = canvas },
        .parent = { .e = e },
        .element = {
            .prefab = prefab_zext,
            .layer = zext_layer,
            .anchor = zext_anchor,
            .position = zext_position
        },
        .zext = {
            .text = label,
            .margins = padding,
            .font_size = font_size,
            .font_resolution = font_size,
            .font_thickness = font_thickness,
            .font_outline_thickness = fonto_thickness,
            .font_fill_color = header_font_fill,
            .font_outline_color = header_font_outline,
        }
    };

    // Children children = (Children) { 0 };

    entity text = spawn_text(world, zext_spawn_data);
    zox_set_parent(world, text, e);

    if (is_close_button) {
        entity close_button = spawn_close_button(world, e, canvas, font_size, padding.y, button_layer, (ClickEvent) { NULL });
        zox_set_parent(world, close_button, e);
    }

    return e;
}
