void on_new_settings_toggle_toggled(
    ecs* world,
    const ToggleEventData* data)
{
    if (!zox_valid(data->e)) {
        zox_loge("Invalid [e] on_new_settings_toggle_toggled");
        return;
    }
    entity e2 = zox_getv(data->e, SettingLink);
    if (!zox_valid(e2)) {
        zox_loge("UI Setting Linked to Invalid [%s]", zox_getn(e2));
        return;
    }
    zox_set(e2, SettingByte, { data->value });
    zox_add(e2, Dirty);
    zox_add(e2, Refresh);   // the save (setting_save_system on Update), once a frame
    // zoxs_set_byte(world, name, data->value);
}

void on_settings_slide(
    ecs* world,
    const SlideEventData* data)
{
    byte dbg_log = 0;
    entity slider = zox_get_parent(world, data->dragged);
    entity text = zox_get_child_by_id(world, slider, zox_id(Text));
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
    zox_add(e2, Dirty);
    zox_add(e2, Refresh);   // the save (setting_save_system on Update), once a frame
    zox_muter(text, TextData, text_data);
    if (!is_zext(text_data, new_text)) {
        set_zext(text_data, new_text);
        zox_add(text, Refresh);
    }
    // zox_log("Slider [%s] and Setting [%s]: %f", zox_get_name(slider), zox_get_name(e2), data->value);
}

// the options menu's tabs: each setting shows on one (its SettingTab tag), the menu shows a tab at a time
#define menu_options_tabs_count 5
const char* menu_options_tab_names[menu_options_tabs_count] = { "Video", "Sound", "World", "Game", "Debug" };
byte menu_options_tab = 0;      // the tab shown, kept while the game runs
// the tabs the menu shows, the ones with settings (a release build has no debug ones): each shown tab's tab
byte menu_options_shown[menu_options_tabs_count];

static inline entity menu_options_tab_id(ecs* world, byte tab) {
    const entity ids[menu_options_tabs_count] = { zox_id(SettingTabVideo), zox_id(SettingTabSound),
        zox_id(SettingTabWorld), zox_id(SettingTabGame), zox_id(SettingTabDebug) };
    return ids[tab < menu_options_tabs_count ? tab : 0];
}

// a setting as listed: sliders first (values to drag), then toggles, each by name
typedef struct {
    entity e;
    byte toggle;
    const char* name;
} menu_option;

static int menu_option_compare(const void* a, const void* b) {
    const menu_option* x = a;
    const menu_option* y = b;
    if (x->toggle != y->toggle) {
        return x->toggle - y->toggle;
    }
    return strcmp(x->name, y->name);
}

#define menu_options_tab_max 64     // settings a tab lists

// the strip's tab picked: remembered for the menu's next opening
void on_menu_options_tab(ecs* world, const ClickEventData event) {
    const int shown = zox_has(event.clicked, ChildIndex) ? zox_getv(event.clicked, ChildIndex) : -1;
    if (shown >= 0 && shown < menu_options_tabs_count) {
        menu_options_tab = menu_options_shown[shown];
    }
}

// a tab's settings as list elements, sorted (sliders, then toggles, each by name), their settings in options;
//  labels hold the sliders' texts. Returns how many
static inline byte menu_options_tab_elements(ecs* world, entity app, entity tab, SpawnListElement* elements,
    char labels[menu_options_tab_max][128], entity* options)
{
    menu_option sorted[menu_options_tab_max];
    byte count = 0;
    iter it = zox_children(world, app);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity e2 = it.entities[i];
            // only those it lists (its types): another kind counted as listed linked past the list's end
            if (!zox_has(e2, Setting) || !zox_has_id(e2, tab) ||
                (!zox_has(e2, SettingToggle) && !zox_has(e2, SettingByteSlider) && !zox_has(e2, SettingFloatSlider))) {
                continue;
            }
            const ZoxName* name = zox_get(e2, ZoxName);
            if (disable_terrain_lods && zox_has(e2, SettingNearRD)) {   // no lods: one distance
                continue;
            }
            if (count >= menu_options_tab_max) {
                zox_loge("Too many options on a tab [%i]", count);
                continue;
            }
            const byte slider = !zox_has(e2, SettingToggle);
            sorted[count++] = (menu_option) { e2, !slider, name->value };
        }
    }
    qsort(sorted, count, sizeof(menu_option), menu_option_compare);
    for (byte k = 0; k < count; k++) {
        const entity e2 = sorted[k].e;
        const ZoxName* name = zox_get(e2, ZoxName);
        options[k] = e2;
        if (zox_has(e2, SettingByteSlider)) {
            const byte value = zox_getv(e2, SettingByte);
            snprintf(labels[k], 128, "%s [%i]", name->value, value);
            elements[k] = (SpawnListElement) {
                .type = list_element_type_slider,
                .on_slide = { &on_settings_slide },
                .text = labels[k],
                .value = (float) value,
                .value_bounds = byte2_to_float2(zox_getv(e2, SettingByteBounds))
            };
        } else if (zox_has(e2, SettingToggle)) {
            elements[k] = (SpawnListElement) {
                .type = list_element_type_toggle,
                .on_toggle = { &on_new_settings_toggle_toggled },
                .text = name->value,
                .value = zox_getv(e2, SettingByte),
            };
        } else {
            const float value = zox_getv(e2, SettingFloat);
            snprintf(labels[k], 128, "%s [%.2f]", name->value, value);
            elements[k] = (SpawnListElement) {
                .type = list_element_type_slider,
                .on_slide = { &on_settings_slide },
                .text = labels[k],
                .value = value,
                .value_bounds = zox_getv(e2, SettingFloatBounds)
            };
        }
    }
    return count;
}

// the options menu: a window of tabs (ui/windows/ins/tabs.c), a tab's settings each, the last tab picked shown
entity spawn_menu_options(
    ecs *world,
    entity player,
    entity canvas,
    int2 position,
    float2 anchor)
{
    if (zox_huds3D) {
        return spawn_menu_options3(world, player);     // a 3D list (XR)
    }
    const byte header_font_size = zox_huds_header_font_size * ui_scale;
    const byte list_font_size = zox_huds_element_font_size * ui_scale;
    const byte2 list_padding = byte2_single(zox_huds_list_padding * ui_scale);
    const entity game = zox_get_parent(world, player);
    const entity app = zox_get_parent(world, game);
    SpawnListElement elements[menu_options_tabs_count][menu_options_tab_max];
    char labels[menu_options_tabs_count][menu_options_tab_max][128];
    entity options[menu_options_tabs_count][menu_options_tab_max];
    entity spawned[menu_options_tabs_count][menu_options_tab_max];
    byte counts[menu_options_tabs_count];
    // the tabs with settings, shown
    const char* names[menu_options_tabs_count];
    SpawnListElement* tab_elements[menu_options_tabs_count];
    entity* tab_spawned[menu_options_tabs_count];
    byte shown_counts[menu_options_tabs_count];
    byte shown = 0, active = 0;
    for (byte t = 0; t < menu_options_tabs_count; t++) {
        counts[t] = menu_options_tab_elements(world, app, menu_options_tab_id(world, t), elements[t], labels[t], options[t]);
        if (!counts[t]) {
            continue;
        }
        active = t == menu_options_tab ? shown : active;
        menu_options_shown[shown] = t;
        names[shown] = menu_options_tab_names[t];
        tab_elements[shown] = elements[t];
        tab_spawned[shown] = spawned[t];
        shown_counts[shown] = counts[t];
        shown++;
    }
    const entity2 e2 = spawn_window_tabs(world, prefab_window, canvas, "Ponder", header_font_size, list_font_size,
        (ClickEvent) { &button_event_menu_main }, zox_huds_window_alignment, zox_huds_window_anchor, list_padding,
        shown, names, tab_elements, shown_counts, 6, active,
        (ClickEvent) { &on_menu_options_tab }, tab_spawned);
    const entity e = e2.x;
    if (!e) {
        return 0;
    }
    zox_name("menu_options");
    zox_add(e, MenuOptions);
    for (byte t = 0; t < menu_options_tabs_count; t++) {
        for (byte k = 0; k < counts[t]; k++) {
            zox_set(spawned[t][k], SettingLink, { options[t][k] });
        }
    }
    return e;
}
