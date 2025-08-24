void on_settings_slider_slid(ecs* world, const SlideEventData* data) {
    zox_geter_value(data->dragged, ParentLink, entity, slider)
    zox_geter_value(slider, SliderLabel, char*, slider_name)
    zoxs_set_float(world, slider_name, data->value);
}

// Options uses a set size that has elements adjust
entity spawn_menu_options(
    ecs *world,
    const entity player,
    const entity canvas,
    const int2 position,
    const float2 anchor
) {
    // more data
    const char* header_label = "Ponder";
    const int max_labels = max_settings;
    const byte visible_count = 12;
    const byte layer = 1;
    const byte font_size = 32;

    // # Window #
    LayoutParentData canvas_data = {
        .e = canvas,
        .size = zox_gett_value(canvas, LayoutSize),
    };
    ElementSpawnData window_element_data = {
        .prefab = prefab_window,
        .position = position,
        .size = (int2) { 520, 580 },
        .anchor = anchor, // float2_half,
        .layer = layer,
    };
    LayoutParentData window_parent_data = {
        .e = canvas_data.e,
        .size = canvas_data.size,
        .position = int2_half(canvas_data.size),
    };
    SpawnWindow2 window_data = {
        .header_text = header_label,
        .header_font_size = 32,
        .header_padding = (byte2) { 8, 12 },
        .is_scrollbar = 0,
    };

    Children children = (Children) { 0 };
    window_data.children = &children;
    const entity e = spawn_window2(
        world,
        canvas_data,
        window_parent_data,
        window_element_data,
        &window_data);
    zox_add_tag(e, MenuOptions);
    zox_name("menu_options");

    // # List #

    SpawnListElement elements[max_labels];
    int elements_count = 0;
    for (uint i = 0; i < settings_count; i++) {
        setting s = settings[i];
        // zox_log("spawning setting %s [%f] [%fx%f]", s.name, s.value_float, s.min_float, s.max_float)
        // set slider label
        if (s.type == zox_data_type_float) {
            elements[elements_count] = (SpawnListElement) {
                .type = 1,
                .text = s.name,
                .on_slide = { &on_settings_slider_slid },
                .value = s.value_float,
                .value_bounds = (float2) { s.min_float, s.max_float },
            };
            elements_count++;
        }
        // todo: support other types
    }
    elements[elements_count] = (SpawnListElement) {
        .text = "return",
        .on_click = { &button_event_menu_main },
    };
    elements_count++;
    LayoutParentData list_parent_data = {
        .e = e,
        .size = window_element_data.size,
        .position = window_element_data.position_in_canvas,
    };
    ElementSpawnData list_element_data = {
        .prefab = prefab_list,
        .position = (int2) { 0, -25 },
        .size = (int2) { window_element_data.size.x, window_element_data.size.y - 50 },
        .anchor = float2_half,
        .layer = layer + 1,
    };
    SpawnList ui_list_data = (SpawnList) {
        .elements = elements,
        .count = elements_count,
        .visible_count = visible_count,
        .font_size = font_size,
        .fill = button_fill,
        .outline = button_outline,
        .slider_height = 64,
        .slider_padding = 64,
        .button_padding = (byte2) { 18, 12 },
        .padding = (byte2) { 18, 18 },
        .margins = (byte2) { 18, 18 },
    };
    const entity list = spawn_list(world,
        canvas_data,
        list_parent_data,
        list_element_data,
        ui_list_data
    );
    add_to_Children(window_data.children, list);
    zox_set_ptr(e, Children, children);

    zox_muter(player, ElementLinks, pelements);
    add_to_ElementLinks(pelements, e);
    zox_set(e, ElementHolder, { player });

    // return our window
    return e;
}