// The options as a 3D list (3D huds, XR: the 2D window was unseen in a headset): the settings, a row each
//  ("Name [value]", toggles On or Off), and Back (the main menu); it scrolls as it's navigated (spawn_list3)
//  - A on a toggle flips it, on a slider steps it up (past its end round to its start); the navigating stick left and
//    right steps a selected slider down and up (huds3_options_stick_system), as a 2D slider's drag sets it
//  - a row's setting is its SettingLink, as the 2D window's elements
#define menu_options3_max 64
#define menu_options3_steps 20.0f    // a float setting's steps, end to end

// its row's text: its name and value
static void setting3_label(ecs* world, entity setting, char* label, int length) {
    const ZoxName* name = zox_get(setting, ZoxName);
    if (zox_has(setting, SettingByte) && zox_has(setting, SettingSlider)) {
        snprintf(label, length, "%s [%i]", name->value, zox_getv(setting, SettingByte));
    } else if (zox_has(setting, SettingByte)) {
        snprintf(label, length, "%s [%s]", name->value, zox_getv(setting, SettingByte) ? "On" : "Off");
    } else if (zox_has(setting, SettingFloat)) {
        snprintf(label, length, "%s [%.2f]", name->value, zox_getv(setting, SettingFloat));
    } else {
        snprintf(label, length, "%s", name->value);
    }
}

// a setting stepped (a toggle flipped), wrapping past its end or held at it; dirtied as the 2D window's do
static void setting3_step(ecs* world, entity setting, int direction, byte wrap) {
    if (zox_has(setting, SettingByte) && zox_has(setting, SettingSlider)) {
        const byte2 bounds = zox_getv(setting, SettingByteBounds);
        int value = (int) zox_getv(setting, SettingByte) + direction;
        if (value > bounds.y) {
            value = wrap ? bounds.x : bounds.y;
        } else if (value < bounds.x) {
            value = wrap ? bounds.y : bounds.x;
        }
        zox_set(setting, SettingByte, { (byte) value });
    } else if (zox_has(setting, SettingByte)) {
        zox_set(setting, SettingByte, { !zox_getv(setting, SettingByte) });
    } else if (zox_has(setting, SettingFloat)) {
        const float2 bounds = zox_getv(setting, SettingFloatBounds);
        const float step = (bounds.y - bounds.x) / menu_options3_steps;
        float value = zox_getv(setting, SettingFloat) + direction * step;
        if (value > bounds.y + step * 0.01f) {
            value = wrap ? bounds.x : bounds.y;
        } else if (value < bounds.x - step * 0.01f) {
            value = wrap ? bounds.y : bounds.x;
        }
        zox_set(setting, SettingFloat, { value });
    } else {
        return;
    }
    zox_set(setting, SettingDirty, { zox_dirty_trigger });
    zox_add(setting, Dirty);
}

// a row's text from its setting (set deferred: the setting's new value is read back from what was set)
static void setting3_relabel(ecs* world, entity row, entity setting) {
    const entity text = zox_get_child_by_id(world, row, zox_id(Text));
    if (!zox_valid(text)) {
        return;
    }
    char label[128];
    setting3_label(world, setting, label, sizeof(label));
    zox_muter(text, TextData, text_data);
    if (!is_zext(text_data, label)) {
        set_zext(text_data, label);
        zox_add(text, Dirty);
    }
}

// steps a row's setting and its text
void setting3_row_step(ecs* world, entity row, int direction, byte wrap) {
    const entity setting = zox_has(row, SettingLink) ? zox_getv(row, SettingLink) : 0;
    if (!zox_valid(setting)) {
        return;
    }
    setting3_step(world, setting, direction, wrap);
}

void button_event_setting3(ecs* world, ClickEventData event) {
    setting3_row_step(world, event.clicked, 1, 1);
}

// the app's settings, as the 2D window lists them
static int menu_options3_settings(ecs* world, entity player, entity* settings, int max) {
    const entity game = zox_get_parent(world, player);
    const entity app = zox_valid(game) ? zox_get_parent(world, game) : 0;
    int count = 0;
    iter it = zox_children(world, app);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && count < max; i++) {
            const entity e2 = it.entities[i];
            if (!zox_has(e2, Setting)) {
                continue;
            }
            if (disable_terrain_lods && !strcmp(zox_get(e2, ZoxName)->value, "Near Distance")) {
                continue;
            }
            if (zox_has(e2, SettingByte) || zox_has(e2, SettingFloat)) {
                settings[count++] = e2;
            }
        }
    }
    return count;
}

entity spawn_menu_options3(ecs* world, entity player) {
    entity settings[menu_options3_max];
    const int count = menu_options3_settings(world, player, settings, menu_options3_max);
    char labels[menu_options3_max][128];
    SpawnListElement elements[menu_options3_max + 1];
    byte length = 0;
    for (int i = 0; i < count; i++) {
        setting3_label(world, settings[i], labels[i], sizeof(labels[i]));
        elements[length++] = (SpawnListElement) { .text = labels[i], .on_click = { &button_event_setting3 } };
    }
    elements[length++] = (SpawnListElement) { .text = "Back", .on_click = { &button_event_menu_main } };
    entity rows[menu_options3_max + 1];
    const entity e = spawn_huds3_list(world, player, "Ponder", zox_huds_header_font_size * ui_scale,
        zox_huds_element_font_size * ui_scale, elements, length, rows);
    for (int i = 0; i < count; i++) {
        zox_set(rows[i], SettingLink, { settings[i] });
    }
    zox_set_unique_name(e, "menu_options");
    zox_add(e, MenuOptions);
    zox_add(e, NavigationWindow);
    return e;
}

// a selected setting row (a 3D list's, its SettingLink) stepped down and up by its navigator's stick left and right,
//  once a push; its text kept to its setting's value
void huds3_options_stick_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(EntityTarget);
    static float last_x = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(EntityTarget, target);
        const entity row = target->value;
        if (!zox_valid(row) || !zox_has(row, SettingLink) || !zox_has(row, Element3)) {
            continue;
        }
        const entity setting = zox_getv(row, SettingLink);
        if (!zox_valid(setting)) {
            continue;
        }
        float x = 0;
        entity zevices[zox_children_capacity];
        const uint zevices_length = zox_get_children(world, e, zevices, zox_children_capacity);
        for (uint k = 0; k < zevices_length; k++) {
            if (zox_has(zevices[k], ZeviceStick) && !zox_has(zevices[k], Disabled)) {
                x += zox_getv(zevices[k], ZeviceStick).x;
            }
        }
        // a stick's x as SDL's gamepads have it (flipped from OpenXR's: xr/fun/controller.c), right is -x
        const float right = -x;
        const byte pushed = fabsf(right) > ui_navigation_joystick_cutoff && fabsf(last_x) <= ui_navigation_joystick_cutoff;
        last_x = right;
        if (pushed && (zox_has(setting, SettingSlider) || zox_has(setting, SettingFloat))) {
            setting3_step(world, setting, right > 0 ? 1 : -1, 0);
        }
        setting3_relabel(world, row, setting);
    }
    zox_sys_on_end();
} zoxd_system(huds3_options_stick_system);
