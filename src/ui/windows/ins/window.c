// NOTE: Spawns window with flat inputs and easy to edit
entity2 spawn_window(ecs *world, entity prefab, entity prefab_body, const char* header, entity parent, int2 position, int2 size, float2 anchor, void* on_closed) {
    color fill = window_fill;
    color outline = window_outline;
    zox_instance(prefab);
    zox_set_unique_name(e, header);
    zox_set_parent(world, e, parent);
    byte header_height = 0;
    {
        byte is_close_button = 1;
        byte header_font_size = 6 * ui_scale;
        if (header == NULL || header[0] == '\0') {
            header_font_size = 0;
        }
        byte2 header_margins = (byte2) { 4 * ui_scale, 3 * ui_scale };
        color header_font_fill = header_font_fill;
        color header_font_outline = header_font_outline;
        header_height = header_font_size + header_margins.y * 2;
        float2 header_anchor = (float2) { 0.5f, 1 };
        int2 header_position = (int2) { 0, -header_height / 2 };
        int2 header_size = (int2) { size.x, header_height };
        spawn_header(world, e, header_position, header_size, header_anchor, header, header_font_size, header_margins, int2_zero, header_size, is_close_button, on_closed);
    }
    int2 body_size = size;
    size.y += header_height;
    float2 anchor_shift = float2_sub(anchor, float2_half); // -0.5 to +0.5
    position.x -= anchor_shift.x * size.x;
    position.y -= anchor_shift.y * size.y;
    zox_set(e, LayoutPosition, { position });
    zox_set(e, Anchor, { anchor });
    zox_set(e, LayoutSize, { size });
    // TODO: Fix WindowToTop by just checking reset WindowLayers when CanvasDirty is flagged
    // TODO: Flag CanvasDirty when new window Initializes
    zox_set(parent, WindowToTop, { e });
    // Body
    int2 body_position = (int2) { 0, -header_height / 2 };
    entity e2 = spawn_uic(world, prefab_body, e, float2_half, body_position, body_size, body_size, fill, outline);
    return (entity2) { e, e2 };
}

// Obsolete
entity2 spawn_window2(ecs *world, LayoutParentData canvas_data, LayoutParentData parent_data, ElementSpawnData element_data, SpawnWindow2 window_data, ClickEvent on_click, byte is_close_button, byte type) {
    byte header_height = window_data.header_font_size + window_data.header_padding.y * 2;
    byte header_font_thickness_s = header_font_thickness * ui_scale;
    byte header_fonto_thickness_s = header_font_thickness * ui_scale;
    zox_instance(element_data.prefab);
    zox_name("window");
    // zox_set(e, WindowType, { type });
    set_element_spawn_data(world, e, canvas_data, parent_data, element_data);
    LayoutParentData e_parent_data = { .e = e };
    entity header;
    {
        // # Window Header #
        // todo: pass more of t this in from top
        ElementSpawnData header_element_data = {
            .prefab = prefab_header,
            .anchor = (float2) { 0.5f, 1.0f },
            .position = (int2) { 0, - header_height / 2 },
            .size = (int2) { element_data.size.x, header_height },
            .layer = element_data.layer + 1,
        };
        SpawnHeaderData header_data = {
            .prefab_zext = prefab_zext,
            .is_close_button = is_close_button
        };
        SpawnTextData header_text_data = {
            .text = window_data.header_text,
            .font_size = window_data.header_font_size,
            .font_resolution = window_data.header_font_size, // header_font_resolution,
            .font_thickness = header_font_thickness_s,
            .font_outline_thickness = header_fonto_thickness_s,
            .font_fill_color = header_font_fill,
            .font_outline_color = header_font_outline,
            .margins = window_data.header_padding,
        };
        header = spawn_header3(world, canvas_data, e_parent_data, header_element_data, header_text_data, header_data, on_click);
        zox_set_parent(world, header, e);
        zox_set(e, HeaderHeight, { header_height });
    }
    // set_window_bounds_to_canvas(world, e, canvas_data.size, element_data.size, element_data.anchor);
    return (entity2) { e, header };
}
