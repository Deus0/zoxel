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
    const int header_font_size = 72;
    const byte visible_count = 4; // 12;
    const byte layer = 1;
    const byte list_font_size = 32;

    SpawnListElement elements[max_settings + 1];
    int elements_count = 0;
    for (uint i = 0; i < max_settings; i++) {
        setting s = settings[i];
        // zox_log("spawning setting %s [%f] [%fx%f]", s.name, s.value_float, s.min_float, s.max_float)
        // set slider label
        if (s.type == zox_data_type_float) {
            elements[elements_count++] = (SpawnListElement) {
                .type = 1,
                .text = s.name,
                .on_slide = { &on_settings_slider_slid },
                .value = s.value_float,
                .value_bounds = (float2) { s.min_float, s.max_float },
            };
        }
        // todo: support other types
    }
    elements[elements_count++] = (SpawnListElement) {
        .text = "Return",
        .on_click = { &button_event_menu_main },
    };

    const entity e = spawn_window_list(
        world,
        player,
        header_label,
        header_font_size,
        elements,
        elements_count,
        visible_count,
        list_font_size
    );
    zox_add_tag(e, MenuOptions);
    zox_name("menu_options");

    return e;
}