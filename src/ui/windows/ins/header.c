entity spawn_header3(ecs *world, LayoutParentData canvas_data, LayoutParentData parent, ElementSpawnData element_data, SpawnTextData zext, SpawnHeaderData header, ClickEvent on_click) {
    zox_instance(element_data.prefab);
    zox_name("header");
    set_element_spawn_data(world, e, canvas_data, parent, element_data);
    zox_set(e, RenderDisabled, { element_data.render_disabled });
    zox_set(e, DraggedLink, { parent.e });
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
    entity text = spawn_text_new(world, prefab_zext, e, zext_position, zext_anchor, zext.font_size, zox_alignment_centre, zext.margins, zext.text, zext.font_fill_color, zext.font_outline_color);
    zox_set_unique_name(text, "header_text");
    // # Header Close Button #
    if (header.is_close_button) {
        entity close_button = spawn_close_button(world, e, canvas_data.e, zext.font_size, zext.margins.y, element_data.layer + 2, on_click);
        zox_set_parent(world, close_button, e);
    }
    return e;
}

entity spawn_header(ecs *world, entity parent, entity canvas, int2 pixel_position, int2 pixel_size, float2 anchor, const char* text, int font_size, int header_margins, byte layer, int2 parent_position, int2 parent_size, byte is_close_button, int2 canvas_size) {
    int string_length = strlen(text);
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
    // byte font_thickness = header_font_thickness * ui_scale;
    // byte fonto_thickness = header_font_thickness * ui_scale;
    // byte zext_layer = layer + 1;
    byte button_layer = layer + 2;
    zox_instance(prefab_header);
    zox_name("header");
    zox_set(e, DraggedLink, { parent });
    initialize_element(world, e, parent, canvas, pixel_position, pixel_size, pixel_size, anchor, layer);

    spawn_text_new(world, prefab_zext, e, zext_position, zext_anchor, font_size, zox_alignment_centre, padding, text, header_font_fill, header_font_outline);

    if (is_close_button) {
        entity close_button = spawn_close_button(world, e, canvas, font_size, padding.y, button_layer, (ClickEvent) { NULL });
        zox_set_parent(world, close_button, e);
    }
    return e;
}
