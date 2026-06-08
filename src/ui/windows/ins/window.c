// NOTE: Spawns window with flat inputs and easy to edit
entity3 spawn_window(ecs* world, entity prefab, entity prefab_body, const char* header, entity parent, int2 position, int2 body_size, float2 anchor, void* on_closed) {
    entity3 output;
    color fill = window_fill;
    color outline = window_outline;
    zox_instance(prefab);
    output.x = e;
    zox_set_unique_name(e, header);
    zox_set_parent(world, e, parent);
    byte header_height = 0;
    // NOTE: Header Anchored to top of window
    {
        byte is_close_button = 1;
        color header_font_fill = header_font_fill;
        color header_font_outline = header_font_outline;
        byte header_font_size = 8 * ui_scale;
        byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
        if (header == NULL || header[0] == '\0') {
            header_font_size = 0;
        }
        header_height = header_font_size + header_padding.y * 2;
        float2 header_anchor = (float2) { 0.5f, 1 };
        int2 header_size = (int2) { body_size.x, header_height };
        int2 header_position = (int2) { 0, -header_height / 2 };
        entity e2 = spawn_header(world, e, header_position, header_size, header_anchor, header, header_font_size, header_padding, is_close_button, on_closed);
        output.y = e2;
    }
    // NOTE: Body Anchored to Bottom of window
    {
        int2 body_position = (int2) { 0, body_size.y / 2 }; //  + 4
        float2 body_anchor = (float2) { 0.5f, 0 };
        entity e2 = spawn_uic(world, prefab_body, e, body_anchor, body_position, body_size, body_size, fill, outline);
        output.z = e2;
    }
    // finish setting window
    int2 size = body_size;
    size.y += header_height;
    // NOTE: [0,1] => [-0.5, +0.5]
    float2 anchor_shift = float2_sub(anchor, float2_half);
    position.x -= anchor_shift.x * size.x;
    position.y -= anchor_shift.y * size.y;
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Anchor, { anchor });
    // TODO: Fix WindowToTop by just checking reset WindowLayers when CanvasDirty is flagged - Flag CanvasDirty when new window Initializes
    zox_set(parent, WindowToTop, { e });
    return output;
}

// OBSOLETE REMOVE
entity2 spawn_window_old(ecs *world, LayoutParentData canvas_data, LayoutParentData parent_data, ElementSpawnData element_data, SpawnWindow2 window_data, ClickEvent on_click, byte is_close_button, byte type) {
    byte header_height = window_data.header_font_size + window_data.header_padding.y * 2;
    byte header_font_thickness_s = header_font_thickness * ui_scale;
    byte header_fonto_thickness_s = header_font_thickness * ui_scale;
    zox_instance(element_data.prefab);
    zox_name("window");
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
        header = spawn_header_old(world, canvas_data, e_parent_data, header_element_data, header_text_data, header_data, on_click);
        zox_set_parent(world, header, e);
    }
    return (entity2) { e, header };
}
