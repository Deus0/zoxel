void on_settings_toggle_toggled(ecs* world, const ToggleEventData* data) {
    if (!zox_valid(data->e)) {
        zox_log_error("Invalid [e]");
        return;
    }

    entity toggle = data->e;
    if (!zox_valid(toggle) || !zox_has(toggle, OptionLabel)) {
        zox_log_error("Invalid [toggle]");
        return;
    }

    zox_geter_value(toggle, OptionLabel, const char*, name);
    zoxs_set_byte(world, name, data->value);
    // zox_log("Toggle Option [%s] set to [%i]", name, data->value);
}

void on_settings_slider_slid_float(ecs* world, const SlideEventData* data) {
    entity slider = zox_get_parent(world, data->dragged);
    // zox_geter_value(data->dragged, ParentLink, entity, slider)
    zox_geter_value(slider, SliderLabel, const char*, slider_name)
    zoxs_set_float(world, slider_name, data->value);
}

void on_settings_slider_slid_int(ecs* world, const SlideEventData* data) {
    entity slider = zox_get_parent(world, data->dragged);
    // zox_geter_value(data->dragged, ParentLink, entity, slider);
    zox_geter_value(slider, SliderLabel, const char*, slider_name);

    // zox_log("Slider %s Value %i", slider_name, data->value);

    zoxs_set_int(world, slider_name, (int) round(data->value));
}

// TODO: Spawn buttons here and just link the setting entity to the buttons

// Options uses a set size that has elements adjust
entity spawn_menu_options(ecs *world, entity player, entity canvas, int2 position, float2 anchor) {

    // more data
    const char* header_label = "Ponder";
    SpawnListElement elements[max_settings + 1];
    int elements_count = 0;
    byte visible_count = 6;

    // Sizing
    byte header_font_size = 18 * ui_scale;
    byte list_font_size = 8 * ui_scale;

    for (uint i = 0; i < settings_count; i++) {
        setting s = settings[i];
        if (!s.name) {
            zox_log_error("Setting Null: [%i] / [%i], Type %i", i, max_settings, s.type);
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
                .on_slide = { &on_settings_slider_slid_float },
                .value = s.value_float,
                .value_bounds = (float2) { s.min_float, s.max_float },
            };
        }

        else if (s.type == zox_data_type_int) {
            float slider_value = (float) (s.value_int - s.min_int) / (s.max_int - s.min_int);

            elements[elements_count++] = (SpawnListElement) {
                .type = list_element_type_slider,
                .text = s.name,
                .on_slide = { &on_settings_slider_slid_int },
                .value =  slider_value,
                .value_bounds = (float2) { (float) s.min_int, (float) s.max_int },
            };
            // zox_logv("New Int Option %s %i %i:%i - f%f", s.name, s.value_int, s.min_int, s.max_int, slider_value);
        }

        // TODO: support other types
    }

    if (zox_valid(settings_manager)) {
        zox_geter(settings_manager, SettingLinks, settings);
        for (int i = 0; i < settings->length; i++) {
            entity setting = settings->value[i];

            if (!zox_valid(setting)) {
                continue;
            }

            zox_geter(setting, ZoxName, name);

            // use entity data instead
            if (zox_has(setting, SettingByte)) {
                zox_geter_value(setting, SettingByte, byte, value);

                elements[elements_count++] = (SpawnListElement) {
                    .type = list_element_type_toggle,
                    .text = name->value,
                    .on_toggle = { &on_settings_toggle_toggled },
                    .value = value,
                };
            }
        }
    }

    entity e = spawn_window_list(world, prefab_window, player, header_label, header_font_size, elements, elements_count, visible_count, list_font_size, (ClickEvent) { &button_event_menu_main }, 1, zox_window_options, 0, zox_alignment_centre, byte2_single(4), NULL).x;
    zox_name("menu_options");

    zox_add_tag(e, MenuOptions);
    zox_add_tag(e, NavigationWindow);

    return e;
}
