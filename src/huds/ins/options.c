void on_settings_toggle_toggled(ecs* world, const ToggleEventData* data) {
    if (!zox_valid(data->e)) {
        zox_log_error("Invalid [e]");
        return;
    }
    // zox_geter_value(data->e, ParentLink, entity, toggle);
    entity toggle = data->e;
    if (!zox_valid(toggle) || !zox_has(toggle, OptionLabel)) {
        zox_log_error("Invalid [toggle]");
        return;
    }
    zox_geter_value(toggle, OptionLabel, char*, name);
    zoxs_set_byte(world, name, data->value);
    zox_log("Toggle Option [%s] set to [%i]", name, data->value);
}

void on_settings_slider_slid(ecs* world, const SlideEventData* data) {
    zox_geter_value(data->dragged, ParentLink, entity, slider)
    zox_geter_value(slider, SliderLabel, char*, slider_name)
    zoxs_set_float(world, slider_name, data->value);
}

// Options uses a set size that has elements adjust
entity spawn_menu_options(ecs *world, entity player, entity canvas, int2 position, float2 anchor) {
    // more data
    const char* header_label = "Ponder";
    const byte visible_count = 6;
    SpawnListElement elements[max_settings + 1];
    int elements_count = 0;

    // Sizing
    byte header_font_size = 18 * ui_scale;
    byte list_font_size = 8 * ui_scale;

    for (uint i = 0; i < max_settings; i++) {
        setting s = settings[i];
        if (!s.name) {
            zox_log_error("setting is null at [%i]", i);
            break;
        }

        // zox_log("spawning setting %s [%i]", s.name, s.type);
        // zox_log("spawning setting %s [%f] [%fx%f]", s.name, s.value_float, s.min_float, s.max_float)

        // Toggle
        if (s.type == zox_data_type_byte) {
            elements[elements_count++] = (SpawnListElement) {
                .type = list_element_type_toggle,
                .text = s.name,
                .on_toggle = { &on_settings_toggle_toggled },
                .value = s.value_byte,
            };
        }

        // Slider
        else if (s.type == zox_data_type_float) {
            elements[elements_count++] = (SpawnListElement) {
                .type = list_element_type_slider,
                .text = s.name,
                .on_slide = { &on_settings_slider_slid },
                .value = s.value_float,
                .value_bounds = (float2) { s.min_float, s.max_float },
            };
        }
        // todo: support other types
    }
    /*elements[elements_count++] = (SpawnListElement) {
        .text = "Return",
        .on_click = { &button_event_menu_main },
    };*/

    entity e = spawn_window_list(
        world,
        prefab_window_invisible,
        player,
        header_label,
        header_font_size,
        elements,
        elements_count,
        visible_count,
        list_font_size,
        (ClickEvent) { &button_event_menu_main },
        1
    );
    zox_name("menu_options");
    zox_add_tag(e, MenuOptions);
    zox_add_tag(e, NavigationWindow);

    return e;
}