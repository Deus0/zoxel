// tst: options_tabs, the options menu shows a tab of settings at a time (huds/ins/settings.c)
//  - opens options: a tab button for each tab, the Video tab shown, every row's setting on it (SettingTabVideo),
//    sliders before toggles, each by name
//  - clicks the Sound tab: the same window (it respawned), only its list shown, every row's setting on it (Volume,
//    Music, SFX)
//  - then world, game, debug and video again, each clicked and shown (settings crashed opening them)
//  - first every slider loaded out of its bounds (a settings file from another version: it crashed): kept in them
//  - every setting is on exactly one tab, of exactly one type (its tag)
//  - tst/options_tabs.ppm: the menu on its sound tab, to look at
//  - run: bash bsh/tst.sh options_tabs
byte zox_tst_otabs_stage = 0;
int zox_tst_otabs_frames = 0;

// the menu's tab button for a tab's tag (its ChildIndex the tab's, huds/ins/settings.c's order), 0 if none
static entity zox_tst_otabs_button(ecs* world, entity e, entity tab) {
    const entity tags[5] = { zox_id(SettingTabVideo), zox_id(SettingTabSound), zox_id(SettingTabWorld),
        zox_id(SettingTabGame), zox_id(SettingTabDebug) };
    int index = -1;
    for (int i = 0; i < 5; i++) {
        index = tags[i] == tab ? i : index;
    }
    // a shown tab's index is of the tabs with settings (menu_options_shown)
    if (zox_has(e, TabButton) && zox_getv(e, ChildIndex) < menu_options_tabs_count &&
        menu_options_shown[zox_getv(e, ChildIndex)] == index) {
        return e;
    }
    entity found = 0;
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && !found; i++) {
            found = zox_tst_otabs_button(world, it.entities[i], tab);
        }
    }
    return found;
}

// the window first opened: picking a tab keeps it (it respawned the menu), only the picked tab's list shown
entity zox_tst_otabs_window = 0;
static void zox_tst_otabs_shown(ecs* world, entity menu, entity tab, const char* when) {
    if (!zox_tst_otabs_window) {
        zox_tst_otabs_window = menu;
    }
    zox_tst_check(menu == zox_tst_otabs_window, "%s: the options menu is a new window, not the one opened", when);
    const entity picked = zox_tst_otabs_button(world, menu, tab);
    const entity strip = zox_valid(picked) ? zox_get_parent(world, picked) : 0;
    int wrong = 0;
    iter it = zox_children(world, strip);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity button = it.entities[i];
            if (!zox_has(button, TabButton)) {
                continue;
            }
            const entity view = zox_getv(button, Target);
            // its list's first row (a scrollview and list don't draw themselves)
            const entity list = zox_valid(view) ? zox_get_link(world, view, ListUILink) : 0;
            entity rows[1];
            const uint count = zox_valid(list) ? get_list_children_ordered(world, list, rows, 1) : 0;
            const byte hidden = count && zox_has(rows[0], RenderDisabled) && zox_getv(rows[0], RenderDisabled);
            wrong += hidden == (button == picked);
        }
    }
    zox_tst_check(picked && !wrong, "%s: [%i] tabs' lists shown or hidden wrong (only the picked one shows)", when, wrong);
}

// every row's setting on the tab, in order: sliders first, by name; returns the rows
static int zox_tst_otabs_rows(ecs* world, entity menu, entity tab, const char* when) {
    const entity view = zox_get_link(world, menu, ScrollviewLink);
    const entity list = zox_valid(view) ? zox_get_link(world, view, ListUILink) : 0;
    if (!zox_valid(list)) {
        zox_tst_check(0, "%s: the options menu has no list", when);
        return 0;
    }
    entity rows[128];
    const uint count = zox_get_children(world, list, rows, 128);
    int off_tab = 0, misordered = 0, settings = 0;
    byte last_toggle = 0;
    const char* last_name = NULL;
    // in ChildIndex order (list_order checks the list keeps it)
    for (uint r = 0; r < count; r++) {
        entity row = 0;
        for (uint k = 0; k < count; k++) {
            if (zox_has(rows[k], ChildIndex) && zox_getv(rows[k], ChildIndex) == (int) r) {
                row = rows[k];
            }
        }
        if (!zox_valid(row) || !zox_has(row, SettingLink)) {
            continue;
        }
        const entity setting = zox_getv(row, SettingLink);
        settings++;
        off_tab += !zox_has_id(setting, tab);
        const byte toggle = zox_has(setting, SettingToggle);
        const char* name = zox_get(setting, ZoxName)->value;
        if (last_name && (toggle < last_toggle || (toggle == last_toggle && strcmp(name, last_name) < 0))) {
            misordered++;
        }
        last_toggle = toggle;
        last_name = name;
    }
    zox_log("[tst] options_tabs %s: [%i] settings, [%i] off its tab, [%i] out of order", when, settings, off_tab, misordered);
    zox_tst_check(settings, "%s: no settings shown", when);
    zox_tst_check(!off_tab, "%s: [%i] settings shown aren't on its tab", when, off_tab);
    zox_tst_check(!misordered, "%s: [%i] settings out of order (sliders, then toggles, by name)", when, misordered);
    return settings;
}

// every toggle the menu shows flipped, as clicked (on_new_settings_toggle_toggled): a setting changed crashed
int zox_tst_otabs_flipped = 0;
static void zox_tst_otabs_flip(ecs* world, entity menu) {
    const entity view = zox_get_link(world, menu, ScrollviewLink);
    const entity list = zox_valid(view) ? zox_get_link(world, view, ListUILink) : 0;
    if (!zox_valid(list)) {
        return;
    }
    entity rows[128];
    const uint count = zox_get_children(world, list, rows, 128);
    for (uint r = 0; r < count; r++) {
        if (!zox_has(rows[r], SettingLink)) {
            continue;
        }
        const entity setting = zox_getv(rows[r], SettingLink);
        if (zox_has(setting, SettingToggle)) {
            on_new_settings_toggle_toggled(world, &(ToggleEventData) { .e = rows[r], .value = !zox_getv(setting, SettingByte) });
            zox_tst_otabs_flipped++;
        }
    }
}

// a settings file from another version: every setting loaded (settings/sys/load.c) out of its bounds, as it
//  saved values an older game allowed; the menu shows them in bounds, it crashed on them (release)
byte zox_tst_otabs_mismatched = 0;
int zox_tst_otabs_out = 0;
static void zox_tst_otabs_mismatch(ecs* world, entity app) {
    zox_tst_otabs_mismatched = 1;
    // gathered first: setting one can make a table (a setting's first Dirty), which ends an iteration over them
    entity settings[256];
    int count = 0;
    iter it = zox_children(world, app);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && count < 256; i++) {
            settings[count++] = it.entities[i];
        }
    }
    for (int i = 0; i < count; i++) {
        const entity e = settings[i];
        if (!zox_has(e, Setting) || !zox_has(e, ZoxName)) {
            continue;
        }
        const char* name = zox_get(e, ZoxName)->value;
        // not those the window or the world would act on badly (a resolution of 255, a scale of 1e6)
        if (zox_has(e, SettingByteSlider)) {
            set_app_setting_byte(world, app, name, 255);
            zox_tst_otabs_out++;
        } else if (zox_has(e, SettingFloat)) {
            set_app_setting_float(world, app, name, 1000);    // its most: a ui scale of 4
            zox_tst_otabs_out++;
        }
    }
    zox_log("[tst] options_tabs: [%i] settings loaded out of their bounds", zox_tst_otabs_out);
}

// every setting in its bounds
static int zox_tst_otabs_out_of_bounds(ecs* world, entity app) {
    int out = 0;
    iter it = zox_children(world, app);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity e = it.entities[i];
            if (zox_has(e, SettingByte) && zox_has(e, SettingByteBounds)) {
                const byte v = zox_getv(e, SettingByte);
                const byte2 b = zox_getv(e, SettingByteBounds);
                out += v < b.x || v > b.y;
                if (v < b.x || v > b.y) {
                    zox_log("[tst]   [%s] [%i] out of [%i %i]", zox_get(e, ZoxName)->value, v, b.x, b.y);
                }
            } else if (zox_has(e, SettingFloat) && zox_has(e, SettingFloatBounds)) {
                const float v = zox_getv(e, SettingFloat);
                const float2 b = zox_getv(e, SettingFloatBounds);
                out += v < b.x || v > b.y;
                if (v < b.x || v > b.y) {
                    zox_log("[tst]   [%s] [%.2f] out of [%.2f %.2f]", zox_get(e, ZoxName)->value, v, b.x, b.y);
                }
            }
        }
    }
    return out;
}

byte zox_tst_options_tabs_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_otabs_frames++;
    zox_tst_progress(zox_tst_otabs_stage * 1000 + zox_tst_otabs_frames);
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const entity options = zox_tst_menu(world, canvas, zox_id(MenuOptions));
    if (zox_tst_otabs_stage == 1 && !zox_tst_otabs_mismatched) {
        zox_tst_otabs_mismatch(world, zox_get_parent(world, zox_get_parent(world, player)));
    }
    if (zox_tst_otabs_stage == 0) {
        zox_tst_waiting = "a mouse click to open the main menu";
        zox_tst_mouse((int2) { 100, 100 }, (zox_tst_otabs_frames / 4) % 2);
        if (!main_menu) {
            return 0;
        }
        zox_tst_otabs_stage = 1;
        zox_tst_otabs_frames = 0;
        return 0;
    }
    if (zox_tst_otabs_stage == 1) {
        zox_tst_mouse((int2) { 100, 100 }, 0);
        if (zox_tst_otabs_frames < 4) {
            return 0;
        }
        if (!zox_tst_menus_click(world, player, main_menu, &button_event_menu_options, "options")) {
            return 1;
        }
        zox_tst_otabs_stage = 2;
        zox_tst_otabs_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the options menu";
    if (!options || zox_tst_otabs_frames < 10) {
        return 0;
    }
    if (zox_tst_otabs_stage == 2) {
        const entity tabs[5] = { zox_id(SettingTabVideo), zox_id(SettingTabSound), zox_id(SettingTabWorld),
            zox_id(SettingTabGame), zox_id(SettingTabDebug) };
        // a button for each tab with settings (a release build has no debug ones)
        int buttons = 0, expected = 0;
        for (byte i = 0; i < 5; i++) {
            buttons += zox_tst_otabs_button(world, options, tabs[i]) != 0;
            int on = 0;
            zox_each_begin(Setting);
            on += zox_has_id(e, tabs[i]) && (zox_has(e, SettingByte) || zox_has(e, SettingFloat));
            zox_each_end();
            expected += on > 0;
        }
        zox_tst_check(buttons == expected, "the options menu has [%i] of its [%i] tab buttons", buttons, expected);
        zox_tst_otabs_rows(world, options, zox_id(SettingTabVideo), "video");
        zox_tst_otabs_shown(world, options, zox_id(SettingTabVideo), "video");
        const entity sound = zox_tst_otabs_button(world, options, zox_id(SettingTabSound));
        if (!sound) {
            return 1;
        }
        zox_log("[tst] clicking [sound]");
        zox_tst_click(world, player, sound);
        zox_tst_otabs_stage = 3;
        zox_tst_otabs_frames = 0;
        return 0;
    }
    if (zox_tst_otabs_stage == 3) {
        const int rows = zox_tst_otabs_rows(world, options, zox_id(SettingTabSound), "sound");
        zox_tst_otabs_shown(world, options, zox_id(SettingTabSound), "sound");
        zox_tst_check(rows == 3, "the sound tab shows [%i] settings, not its 3 (Volume, Music, SFX)", rows);
        // to look at: tst/options_tabs.ppm
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("options_tabs.ppm"));
        zox_tst_otabs_stage = 4;
        return 0;
    }
    if (zox_screenshot_request[0]) {
        return 0;
    }
    // then the rest, each clicked and shown (world, game, debug), back to video
    const entity rest[4] = { zox_id(SettingTabWorld), zox_id(SettingTabGame), zox_id(SettingTabDebug), zox_id(SettingTabVideo) };
    const char* rest_names[4] = { "world", "game", "debug", "video again" };
    const int k = zox_tst_otabs_stage - 4;   // even: click, odd: check
    if (k >= 8) {
        return 1;
    }
    if (k % 2 == 0) {
        const entity button = zox_tst_otabs_button(world, options, rest[k / 2]);
        if (!button && k / 2 == 2) {
            // no debug settings (a release build): no debug tab
            zox_tst_otabs_stage += 2;
            return 0;
        }
        zox_tst_check(button, "no [%s] tab button", rest_names[k / 2]);
        if (!button) {
            return 1;
        }
        zox_tst_click(world, player, button);
    } else {
        zox_tst_otabs_rows(world, options, rest[k / 2], rest_names[k / 2]);
        zox_tst_otabs_shown(world, options, rest[k / 2], rest_names[k / 2]);
        zox_tst_otabs_flip(world, options);
    }
    zox_tst_otabs_stage++;
    zox_tst_otabs_frames = 0;
    return 0;
}

void zox_tst_options_tabs_check(ecs* world) {
    const entity app = zox_get_parent(world, zox_get_parent(world, dbg_player));
    const int out = zox_tst_otabs_out_of_bounds(world, app);
    zox_tst_check(!out, "[%i] settings loaded out of their bounds stayed out", out);
    // every setting on one tab
    const entity tabs[5] = { zox_id(SettingTabVideo), zox_id(SettingTabSound), zox_id(SettingTabWorld),
        zox_id(SettingTabGame), zox_id(SettingTabDebug) };
    int settings = 0, wrong = 0, untyped = 0;
    zox_each_begin(Setting);
        int on = 0;
        for (byte i = 0; i < 5; i++) {
            on += zox_has_id(e, tabs[i]);
        }
        settings++;
        if (on != 1) {
            wrong++;
            zox_log("[tst] setting [%s] is on [%i] tabs", zox_get(e, ZoxName)->value, on);
        }
        // and of one type (a tag each: SettingToggle, SettingByteSlider, SettingFloatSlider)
        const int types = zox_has(e, SettingToggle) + zox_has(e, SettingByteSlider) + zox_has(e, SettingFloatSlider);
        if (types != 1) {
            untyped++;
            zox_log("[tst] setting [%s] has [%i] type tags", zox_get(e, ZoxName)->value, types);
        }
    zox_each_end();
    zox_tst_check(settings, "no settings");
    zox_tst_check(!wrong, "[%i] of [%i] settings aren't on exactly one tab", wrong, settings);
    zox_tst_check(!untyped, "[%i] of [%i] settings haven't exactly one type tag", untyped, settings);
}
