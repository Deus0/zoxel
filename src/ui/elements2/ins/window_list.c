// List Menus adjust to the menu size
entity spawn_window_list(
    ecs *world,
    const entity player,
    const char *header,
    const byte header_font_size,
    SpawnListElement* elements,
    byte elements_count,
    byte visible_count,
    byte list_font_size,
    ClickEvent close_event,
    byte is_close_button
) {
    const byte window_layer = 3;    // does tihs matter? should get sorted after anyway?

    zox_geter_value(player, CanvasLink, entity, canvas);

    // # Window #
    LayoutParentData canvas_data = {
        .e = canvas,
        .size = zox_gett_value(canvas, LayoutSize)  // need for bounds
    };
    ElementSpawnData window_element_data = {
        .prefab = prefab_window_invisible,
        .anchor = float2_half,
        .layer = window_layer,
    };
    SpawnWindow2 window_data = {
        .header_text = header,
        .header_font_size = header_font_size,
        .header_padding = (byte2) { 24, 8 },
        .is_scrollbar = 0,
    };
    // we need to calculate header size too
    int2 header_size = calculate_header_size(
        strlen(header),
        window_data.header_font_size,
        window_data.header_padding);
    int header_height = header_size.y;

    // # List #
    SpawnList list_data = (SpawnList) {
        .visible_count = visible_count,
        .count = elements_count,
        .elements = elements,
        .font_size = list_font_size,
        .fill = button_fill,
        .outline = button_outline,
        .button_padding = (byte2) { 32, 16 },
        .padding = (byte2) { 18, 24 },
        .margins = (byte2) { 64, 32 },
        .slider_height = 64,
        .slider_padding = 64,
    };

    // Our window again, spawn using list size
    // calculate size
    int2 list_size = calculate_list_size(
        calculate_list_max_characters(list_data),
        list_data
    );
    // we use the bigger size out of list and header widths
    window_element_data.size = (int2) {
        int_max(list_size.x, header_size.x),
        list_size.y + header_height
    };

    Children window_children = (Children) { 0 };
    window_data.children = &window_children;

    // Spawn our Window
    const entity e = spawn_window2(
        world,
        canvas_data,
        (LayoutParentData) { .e = canvas },
        window_element_data,
        window_data,
        close_event,
        is_close_button
    );

    // Spawn Scrollview
    ElementSpawnData scrollview_data = {
        .prefab = prefab_element_invisible,
        .position = (int2) {
            0,
            -header_height / 2
        },
        .size = list_size,
        .anchor = float2_half,
        .layer = window_layer + 1,
    };
    Children scrollview_children = { 0 };
    entity scrollview = spawn_scrollview(
        world,
        canvas_data,
        (LayoutParentData) { .e = e },
        scrollview_data,
        &scrollview_children,
        list_data.visible_count,
        list_data.count
    );
    add_to_Children(&window_children, scrollview);
    zox_set_ptr(e, Children, window_children);

    // Spawn our list
    ElementSpawnData list_element_data = {
        .prefab = prefab_list,
        .size = list_size,
        .anchor = float2_half,
        .layer = window_layer + 2,
    };
    const entity list = spawn_list(
        world,
        canvas_data,
        (LayoutParentData) { .e = scrollview },
        list_element_data,
        list_data
    );
    add_to_Children(&scrollview_children, list);
    zox_set_ptr(scrollview, Children, scrollview_children);
    zox_muter(player, ElementLinks, pelements);
    add_to_ElementLinks(pelements, e);
    zox_set(e, ElementHolder, { player });

    return e;
}