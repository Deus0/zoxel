entity spawn_window2(
    ecs *world,
    const LayoutParentData canvas_data,
    const LayoutParentData parent_data,
    const ElementSpawnData element_data,
    SpawnWindow2 window_data,
    ClickEvent on_click,
    byte is_close_button
) {
    const byte header_height = window_data.header_font_size + window_data.header_padding.y * 2;

    zox_instance(element_data.prefab);
    zox_name("window");
    set_element_spawn_data(
        world,
        e,
        canvas_data,
        parent_data,
        element_data
    );
    zox_set(e, HeaderHeight, { header_height });

    // start children
    Children* children = window_data.children;

    const LayoutParentData e_parent_data = {
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
        .font_resolution = header_font_resolution,
        .font_thickness = header_font_thickness_fill,
        .font_outline_thickness = header_font_thickness_outline,
        .font_fill_color = header_font_fill,
        .font_outline_color = header_font_outline,
        .margins = window_data.header_padding,
    };

    const entity header = spawn_header3(
        world,
        canvas_data,
        e_parent_data,
        header_element_data,
        header_text_data,
        header_data,
        on_click
    );
    add_to_Children(children, header);

    set_window_bounds_to_canvas(
        world,
        e,
        canvas_data.size,
        element_data.size,
        element_data.anchor
    );

    return e;
}