entity spawn_window2(ecs *world, LayoutParentData canvas_data, LayoutParentData parent_data, ElementSpawnData element_data, SpawnWindow2 window_data, ClickEvent on_click, byte is_close_button, byte type) {

    byte header_height = window_data.header_font_size + window_data.header_padding.y * 2;
    byte header_font_thickness_s = header_font_thickness * ui_scale;
    byte header_fonto_thickness_s = header_font_thickness * ui_scale;

    zox_instance(element_data.prefab);
    zox_name("window");

    zox_set(e, WindowType, { type });

    set_element_spawn_data(world, e, canvas_data, parent_data, element_data);

    zox_set(e, HeaderHeight, { header_height });

    // start children
    Children* children = window_data.children;

    LayoutParentData e_parent_data = {
        .e = e,
        .size = element_data.size,
    };

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

    entity header = spawn_header3(world, canvas_data, e_parent_data, header_element_data, header_text_data, header_data, on_click);
    add_to_Children(children, header);

    set_window_bounds_to_canvas(world, e, canvas_data.size, element_data.size, element_data.anchor);

    return e;
}

// TODO: Spawn header in HeaderSpawnSystem
entity spawn_window(ecs *world, entity p, entity canvas, entity parent, byte wtype, int2 position, int2 size, float2 anchor) {

    zox_instance(p);
    zox_name("window");

    zox_set(e, WindowType, { wtype });
    zox_set(e, ParentLink, { parent });
    zox_set(e, CanvasLink, { canvas });
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Anchor, { anchor });
    // zox_set(e, Layer2D, { layer });

    // TODO: Children Dirty Flag?
    zox_set(canvas, WindowToTop, { e });
    set_window_bounds_to_canvas(world, e, zox_gett_value(canvas, LayoutSize), size, anchor);


    return e;
}

entity spawn_window_header(ecs* world, entity canvas, entity window, int2 wsize, const char* text, byte font_size, byte2 padding, byte font_thickness, byte fonto_thickness, ClickEvent on_close) {

    byte can_close = on_close.value != NULL;
    int2 size = (int2) { wsize.x, font_size + padding.y * 2 };
    int2 position = (int2) { 0, size.y / 2 };
    float2 anchor = (float2) { 0.5f, 1.0f };
    byte font_thickness_s = font_thickness * ui_scale;
    byte fonto_thickness_s = fonto_thickness * ui_scale;

    // # Window Header #
    // todo: pass more of t this in from top
    LayoutParentData canvas_data = {
        .e = canvas
    };
    LayoutParentData parent_data = {
        .e = window,
    };
    ElementSpawnData edata = {
        .prefab = prefab_header,
        .anchor = anchor,
        .position = position,
        .size = size,
    };
    SpawnTextData tdata = {
        .text = text,
        .font_resolution = font_size,
        .font_size = font_size,
        .margins = padding,
        .font_thickness = font_thickness_s,
        .font_outline_thickness = fonto_thickness_s,
        .font_fill_color = header_font_fill,
        .font_outline_color = header_font_outline,
    };
    SpawnHeaderData hdata = {
        .prefab_zext = prefab_zext,
        .is_close_button = can_close
    };

    zox_set(window, HeaderHeight, { size.y });
    zox_log("header spawned at: %ix%i", position.x, position.y);

    return spawn_header3(world, canvas_data, parent_data, edata, tdata, hdata, on_close);
}
