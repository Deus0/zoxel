

void on_new_settings_toggle_toggled(ecs* world, const ToggleEventData* data) {
    if (!zox_valid(data->e)) {
        zox_log_error("Invalid [e] on_new_settings_toggle_toggled");
        return;
    }
    entity e2 = zox_getv(data->e, SettingLink);
    if (!zox_valid(e2)) {
        zox_loge("UI Setting Linked to Invalid [%s]", zox_getn(e2));
        return;
    }
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
        zox_setm(text, TextDirty, zox_dirty_trigger);
        // zox_muter(text, TextDirty, dirty);
        // dirty->value = zox_dirty_trigger;
    }
    // zox_log("Slider [%s] and Setting [%s]: %f", zox_get_name(slider), zox_get_name(e2), data->value);
}

// TODO: Spawn buttons here and just link the setting entity to the buttons
// Options uses a set size that has elements adjust
entity spawn_menu_options(ecs *world, entity player, entity canvas, int2 position, float2 anchor) {
    byte dbg_log = 0;
    byte window_alignment = zox_huds_window_alignment;
    float2 window_anchor = zox_huds_window_anchor;
    byte header_font_size = zox_huds_header_font_size * ui_scale;
    byte list_font_size = zox_huds_element_font_size * ui_scale;
    byte2 list_padding = byte2_single(zox_huds_list_padding * ui_scale);
    byte visible_count = 6;
    const char* header_label = "Ponder";
    SpawnListElement elements[max_settings];
    int elements_count = 0;
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
    entity e = spawn_window_list(world, prefab_window, player, header_label, header_font_size, list_font_size, (ClickEvent) { &button_event_menu_main }, 1, 0, 0, window_alignment, window_anchor, list_padding, spawned_elements, elements, elements_count, visible_count).x;
    zox_name("menu_options");
    zox_add_tag(e, MenuOptions);
    zox_add_tag(e, NavigationWindow);
    for (int i = 0; i < options_count; i++) {
        entity e2 = spawned_elements[i];
        entity setting = options[i];
        zox_set(e2, SettingLink, { setting });
    }
    return e;
}
