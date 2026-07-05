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
    zox_geter_value(slider, SliderLabel, const char*, slider_name)
    zoxs_set_float(world, slider_name, data->value);
}

void on_settings_slider_slid_int(ecs* world, const SlideEventData* data) {
    entity slider = zox_get_parent(world, data->dragged);
    zox_geter_value(slider, SliderLabel, const char*, slider_name);
    // zox_log("Slider %s Value %i", slider_name, data->value);
    zoxs_set_int(world, slider_name, (int) round(data->value));
}

void on_new_settings_toggle_toggled(ecs* world, const ToggleEventData* data) {
    if (!zox_valid(data->e)) {
        zox_log_error("Invalid [e]");
        return;
    }
    entity e2 = zox_getv(data->e, SettingLink);
    zox_set(e2, SettingByte, { data->value });
    zox_set(e2, SettingDirty, { zox_dirty_trigger });
    // zoxs_set_byte(world, name, data->value);
}

void on_settings_slide(ecs* world, const SlideEventData* data) {
    byte dbg_log = 0;
    entity slider = zox_get_parent(world, data->dragged);
    entity text = zox_get_child_by_id(world, slider, zox_id(Text2D));
    entity e2 = zox_getv(slider, SettingLink);
    if (!zox_valid(e2) || !zox_valid(slider) || !zox_valid(text)) {
        return;
    }
    char new_text[128];
    const ZoxName* name = zox_get(e2, ZoxName);
    if (zox_has(e2, SettingByte)) {
        // byte2 bounds = zox_getv(e2, SettingByteBounds);
        byte value = round(data->value); // bounds.x + round(data->value * (bounds.y - bounds.x));
        zox_muter(e2, SettingByte, setting);
        if (setting->value == value) {
            return;
        }
        setting->value = value;
        snprintf(new_text, 128, "%s [%i]", name->value, value);
        if (dbg_log) {
            zox_log("--Setting Byte Slider [%s]:[%i] - Slide [%f]", zox_get_name(e2), value, data->value); // , bounds.x, bounds.y
        }
    } else if (zox_has(e2, SettingFloat)) {
        zox_set(e2, SettingFloat, { data->value });
        snprintf(new_text, 128, "%s [%.2f]", name->value, data->value);
    }
    zox_set(e2, SettingDirty, { zox_dirty_trigger });
    zox_muter(text, TextData, text_data);
    if (!is_zext(text_data, new_text)) {
        set_zext(text_data, new_text);
        zox_muter(text, TextDirty, dirty);
        dirty->value = zox_dirty_trigger;
    }
    // zox_log("Slider [%s] and Setting [%s]: %f", zox_get_name(slider), zox_get_name(e2), data->value);
}

// TODO: Spawn buttons here and just link the setting entity to the buttons
// Options uses a set size that has elements adjust
entity spawn_menu_options(ecs *world, entity player, entity canvas, int2 position, float2 anchor) {
    byte dbg_log = 0;
    // more data
    const char* header_label = "Ponder";
    SpawnListElement elements[max_settings + 1];
    int elements_count = 0;
    byte visible_count = 6;
    byte header_font_size = 18 * ui_scale;
    byte list_font_size = 6 * ui_scale;
    byte2 padding = byte2_single(6 * ui_scale);
    // Sizing
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
    }
    entity game = zox_get_parent(world, player);
    entity app = zox_get_parent(world, game);
    uint options_count = 0;
    byte label_length = 128;
    entity options[128];
    char options_labels[128][label_length];
    iter it = zox_children(world, app);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            if (!zox_has(e2, Setting)) {
                continue;
            }
            if (options_count >= 128) {
                zox_loge("Too many options [%i]", options_count);
                options_count++;
                continue;
            }
            const ZoxName* name = zox_get(e2, ZoxName);
            if (disable_terrain_lods && !strcmp(name->value, "Near Distance")) {
                continue;
            }
            options[options_count] = e2;
            char* label = options_labels[options_count];
            if (zox_has(e2, SettingByte) && zox_has(e2, SettingSlider)) {
                byte value = zox_getv(e2, SettingByte);
                byte2 bounds = zox_getv(e2, SettingByteBounds);
                snprintf(label, label_length, "%s [%i]", name->value, value);
                elements[elements_count++] = (SpawnListElement) {
                    .type = list_element_type_slider,
                    .on_slide = { &on_settings_slide },
                    .text = label,
                    .value = (float) value,
                    .value_bounds = byte2_to_float2(bounds)
                };
                if (dbg_log) {
                    zox_log("New Setting Byte Slider [%s]:[%i] - [%i,%i]", zox_get_name(e2), value, bounds.x, bounds.y);
                }
            } else if (zox_has(e2, SettingByte)) {
                byte value = zox_getv(e2, SettingByte);
                elements[elements_count++] = (SpawnListElement) {
                    .type = list_element_type_toggle,
                    .on_toggle = { &on_new_settings_toggle_toggled },
                    .text = name->value,
                    .value = value,
                };
                if (dbg_log) {
                    zox_log("New Setting Byte Toggle [%s]:[%i] ", zox_get_name(e2), value);
                }
            } else if (zox_has(e2, SettingFloat)) {
                float value = zox_getv(e2, SettingFloat);
                float2 bounds = zox_getv(e2, SettingFloatBounds);
                snprintf(label, label_length,"%s [%.2f]", name->value, value);
                elements[elements_count++] = (SpawnListElement) {
                    .type = list_element_type_slider,
                    .on_slide = { &on_settings_slide },
                    .text = label, // name->value,
                    .value = value,
                    .value_bounds = bounds
                };
                if (dbg_log) {
                    zox_log("New Setting Float Slider [%s]:[%f] - [%f,%f]", zox_get_name(e2), value, bounds.x, bounds.y);
                }
            }
            options_count++;
        }
    }
    entity spawned_elements[elements_count];
    entity e = spawn_window_list(world, prefab_window, player, header_label, header_font_size, list_font_size, (ClickEvent) { &button_event_menu_main }, 1, 0, 0, zox_alignment_centre, padding, spawned_elements, elements, elements_count, visible_count).x;
    zox_name("menu_options");
    zox_add_tag(e, MenuOptions);
    zox_add_tag(e, NavigationWindow);
    for (int i = 0; i < elements_count; i++) {
        if (i < settings_count) {
            continue;
        }
        int j = i - settings_count;
        entity e2 = spawned_elements[i];
        entity setting = options[j];
        zox_set(e2, SettingLink, { setting });
    }
    return e;
}
