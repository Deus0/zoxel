entity spawn_header(
    ecs *world,
    entity parent,
    int2 position,
    int2 size,
    float2 anchor,
    const char* text,
    int font_size,
    byte2 padding,
    byte is_close_button,
    void* close_event)
{
    int string_length = strlen(text);
    int2 text_position = int2_zero;
    float2 text_anchor = (float2) { 0, 0.5f };
    if (!is_close_button) {
        // Centre Alignment
        text_position.x = 0;
        text_anchor.x = 0.5f;
    } else {
        // Left Alignment
        text_position.x = ((font_size * string_length) / 2) + padding.x / 2;
    }
    zox_instance(prefab_header);
    zox_name("header");
    zox_set_parent(world, e, parent);
    zox_setv(e, LayoutPosition, position);
    zox_setv(e, LayoutSize, size);
    zox_setv(e, Anchor, anchor);
    zox_link(world, e, DraggedLink, parent);
    {
        spawn_text(
            world,
            prefab_text,
            e,
            text_position,
            text_anchor,
            font_size,
            zox_alignment_centre,
            padding,
            text,
            header_font_fill,
            header_font_outline);
    }
    if (is_close_button) {
        // Just do spawn_button
        spawn_close_button(
            world,
            e,
            0,
            font_size,
            padding.y,
            0,
            (ClickEvent) {
                close_event
            });
    }
    return e;
}

// OBSOLETE
entity spawn_header_old(
    ecs *world,
    LayoutParentData canvas_data,
    LayoutParentData parent,
    ElementSpawnData element_data,
    SpawnTextData zext,
    SpawnHeaderData header,
    ClickEvent on_click)
{
    zox_instance(element_data.prefab);
    zox_name("header");
    set_element_spawn_data(
        world,
        e,
        canvas_data,
        parent,
        element_data);
    zox_set(e, RenderDisabled, { element_data.render_disabled });
    zox_link(world, e, DraggedLink, parent.e);
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
    entity text = spawn_text(
        world,
        prefab_text,
        e,
        zext_position,
        zext_anchor,
        zext.font_size,
        zox_alignment_centre,
        zext.margins,
        zext.text,
        zext.font_fill_color,
        zext.font_outline_color);
    zox_set_unique_name(text, "header_text");
    // # Header Close Button #
    if (header.is_close_button) {
        entity close_button = spawn_close_button(
            world,
            e,
            canvas_data.e,
            zext.font_size,
            zext.margins.y,
            element_data.layer + 2,
            on_click);
        zox_set_parent(world, close_button, e);
    }
    return e;
}
