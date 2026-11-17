// tst: xr_options, the options menu on 3D huds (huds/ins/settings3.c) by the XR controllers alone, on the simulator
//  - A on the start menu, the stick down to Options on the main menu, A: the options are a 3D list (Canvas3: the 2D
//    window was unseen in a headset)
//  - A on its first setting: the setting's value changes, its row's text shows it
//  - the stick down to a slider, then right: its value steps up (or stays at its end), its text shows it
//  - the stick down to Back, A: the main menu again; the list scrolled on the way (more rows than it shows), its
//    selection always shown (spawn_window_list, scroll_list_to_child)
//  - run: bash bsh/tst.sh xr_options (screenshot: the list scrolled to Back, xr_options.ppm)
int zox_tst_xr_options_stage = 0;
int zox_tst_xr_options_frames = 0;
byte zox_tst_xr_options_menu3 = 0;
entity zox_tst_xr_options_setting = 0;
float zox_tst_xr_options_before = 0;
byte zox_tst_xr_options_clicked = 0;        // A changed the first setting, its text with it
byte zox_tst_xr_options_slid = 0;           // the stick stepped a slider, its text with it
byte zox_tst_xr_options_back = 0;
byte zox_tst_xr_options_scrolled = 0;       // its list's ListStart moved off 0
byte zox_tst_xr_options_hidden = 0;
byte zox_tst_xr_options_handle_end = 0;     // scrolled to its end, the scrollbar's handle at its bottom         // a selected row hidden (not scrolled to) a while
int zox_tst_xr_options_hidden_frames = 0;
int zox_tst_xr_options_shot = 0;            // the frame Back was first selected
int zox_tst_xr_options_moves = 0;

static inline void zox_tst_xr_options_next() {
    zox_tst_xr_options_stage++;
    zox_tst_xr_options_frames = 0;
}

static inline entity zox_tst_xr_options_target(ecs* world) {
    const entity right = xr_controllers[xr_hand_right];
    return zox_valid(right) ? zox_getv(right, EntityTarget) : 0;
}

static inline float zox_tst_xr_options_value(ecs* world, entity setting) {
    if (zox_has(setting, SettingByte)) {
        return zox_getv(setting, SettingByte);
    }
    return zox_has(setting, SettingFloat) ? zox_getv(setting, SettingFloat) : 0;
}

// its row's text is its setting's label
static inline byte zox_tst_xr_options_text_matches(ecs* world, entity row, entity setting) {
    const entity text = zox_get_child_by_id(world, row, zox_id(Text));
    if (!zox_valid(text)) {
        return 0;
    }
    char label[128];
    setting3_label(world, setting, label, sizeof(label));
    return is_zext((TextData*) zox_get(text, TextData), label);
}

// the stick down a moment (the selection one row down), then let go, every 16 frames
static inline void zox_tst_xr_options_down(int f) {
    xr_sim_script_stick = (f % 16 < 3) ? (float2) { 0, -1 } : float2_zero;
}

byte zox_tst_xr_options_ready(ecs* world) {
    const entity player = zox_players[0];
    if (!zox_valid(xr_camera) || !zox_valid(player)) {
        zox_tst_waiting = "the xr camera";
        return 0;
    }
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const int f = ++zox_tst_xr_options_frames;
    zox_tst_progress(zox_tst_xr_options_stage * 1000 + f);
    const entity target = zox_tst_xr_options_target(world);
    xr_sim_script_a = 0;
    if (zox_valid(target) && zox_has(target, ChildIndex) && zox_has(target, Element3)) {
        const entity list = zox_get_parent(world, target);
        if (zox_valid(list) && zox_has(list, ListStart) && zox_getv(list, ListStart) > 0) {
            zox_tst_xr_options_scrolled = 1;
        }
        // a frame or two behind the selection (deferred), never more
        zox_tst_xr_options_hidden_frames = zox_getv(target, RenderDisabled) ? zox_tst_xr_options_hidden_frames + 1 : 0;
        if (zox_tst_xr_options_hidden_frames > 6) {
            zox_tst_xr_options_hidden = 1;
        }
    }
    switch (zox_tst_xr_options_stage) {
        case 0:
            zox_tst_waiting = "the start menu";
            xr_sim_script_a = f >= 20 && f < 24;
            if (f > 30 && !zox_valid(zox_get_link(world, player, MenuStart))) {
                zox_tst_xr_options_next();
            }
            return 0;
        case 1:
            zox_tst_waiting = "Options on the main menu";
            if (f < 20) {
                return 0;
            }
            {
                const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuOptions));
                if (zox_valid(menu)) {
                    zox_tst_xr_options_menu3 = zox_has(menu, Canvas3);
                    xr_sim_script_stick = float2_zero;
                    zox_tst_xr_options_next();
                    return 0;
                }
            }
            if (zox_valid(target) && zox_has(target, ClickEvent) && zox_getv(target, ClickEvent) == &button_event_menu_options) {
                xr_sim_script_stick = float2_zero;
                xr_sim_script_a = f % 30 >= 20 && f % 30 < 24;
            } else {
                zox_tst_xr_options_down(f);
            }
            return 0;
        case 2:
            zox_tst_waiting = "the first setting selected";
            xr_sim_script_stick = float2_zero;
            if (f > 20 && zox_valid(target) && zox_has(target, SettingLink)) {
                zox_tst_xr_options_setting = zox_getv(target, SettingLink);
                zox_tst_xr_options_before = zox_tst_xr_options_value(world, zox_tst_xr_options_setting);
                zox_tst_xr_options_next();
            }
            return 0;
        case 3:
            zox_tst_waiting = "A on it";
            xr_sim_script_a = f >= 2 && f < 6;
            if (f > 20) {
                zox_tst_xr_options_clicked = zox_tst_xr_options_value(world, zox_tst_xr_options_setting) != zox_tst_xr_options_before &&
                    zox_tst_xr_options_text_matches(world, target, zox_tst_xr_options_setting);
                zox_tst_xr_options_next();
            }
            return 0;
        case 4:
            zox_tst_waiting = "a slider selected";
            if (zox_valid(target) && zox_has(target, SettingLink) && f > 10) {
                const entity setting = zox_getv(target, SettingLink);
                if (zox_has(setting, SettingFloat) || zox_has(setting, SettingSlider)) {
                    xr_sim_script_stick = float2_zero;
                    zox_tst_xr_options_setting = setting;
                    zox_tst_xr_options_before = zox_tst_xr_options_value(world, setting);
                    zox_tst_xr_options_next();
                    return 0;
                }
            }
            zox_tst_xr_options_down(f);
            if (f > 600) {
                zox_tst_xr_options_setting = 0;
                zox_tst_xr_options_next();  // no slider found
            }
            return 0;
        case 5:
            zox_tst_waiting = "the stick right on it";
            xr_sim_script_stick = f >= 4 && f < 8 ? (float2) { 1, 0 } : float2_zero;
            if (f > 24) {
                const entity setting = zox_tst_xr_options_setting;
                if (zox_valid(setting)) {
                    const float now = zox_tst_xr_options_value(world, setting);
                    const float end = zox_has(setting, SettingFloat) ? zox_getv(setting, SettingFloatBounds).y :
                        zox_getv(setting, SettingByteBounds).y;
                    zox_tst_xr_options_slid = (now > zox_tst_xr_options_before || now >= end - 0.0001f) &&
                        zox_tst_xr_options_text_matches(world, target, setting);
                    zox_log("[tst] xr_options: the slider [%s] from [%.3f] to [%.3f]", zox_get(setting, ZoxName)->value, zox_tst_xr_options_before, now);
                }
                zox_tst_xr_options_next();
            }
            return 0;
        case 6:
            zox_tst_waiting = "Back";
            if (zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuMain)))) {
                zox_tst_xr_options_back = 1;
                xr_sim_script_stick = float2_zero;
                return 1;
            }
            if (zox_valid(target) && zox_has(target, ClickEvent) && zox_getv(target, ClickEvent) == &button_event_menu_main) {
                xr_sim_script_stick = float2_zero;
                if (f == zox_tst_xr_options_shot + 10) {
                    // the scrollbar's handle at its bottom (scroll_list_to_child moves it, as a 2D list's)
                    const entity list = zox_get_parent(world, target);
                    const entity scrollview = zox_valid(list) ? zox_get_link(world, list, ScrollviewLink) : 0;
                    const entity handle = zox_valid(scrollview) ? get_scrollview_handle(world, scrollview) : 0;
                    const entity bar = zox_valid(handle) ? zox_get_parent(world, handle) : 0;
                    if (zox_valid(bar)) {
                        const int travel = zox_getv(bar, LayoutSize).y - zox_getv(handle, LayoutSize).y;
                        zox_tst_xr_options_handle_end = zox_getv(handle, LayoutPosition).y <= -travel / 2 + 1;
                        zox_log("[tst] xr_options: the handle at [%i] of its travel [%i]", zox_getv(handle, LayoutPosition).y, travel);
                    }
                }
                // scrolled to its end, Back selected: a screenshot (tst/xr_options.ppm), then A
                if (!zox_tst_xr_options_shot) {
                    zox_tst_xr_options_shot = f;
                } else if (f == zox_tst_xr_options_shot + 10) {
                    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("xr_options.ppm"));
                }
                xr_sim_script_a = f >= zox_tst_xr_options_shot + 20 && f < zox_tst_xr_options_shot + 24;
            } else {
                zox_tst_xr_options_down(f);
            }
            return f > 1200;
    }
    return 0;
}

void zox_tst_xr_options_check(ecs* world) {
    (void) world;
    zox_log("[tst] xr_options: 3D [%i], A changed a setting [%i], the stick a slider [%i], scrolled [%i], a hidden selection [%i], back [%i]",
        zox_tst_xr_options_menu3, zox_tst_xr_options_clicked, zox_tst_xr_options_slid, zox_tst_xr_options_scrolled,
        zox_tst_xr_options_hidden, zox_tst_xr_options_back);
    zox_tst_check(zox_tst_xr_options_menu3, "the options menu isn't a 3D list");
    zox_tst_check(zox_tst_xr_options_clicked, "A didn't change the setting (or its text)");
    zox_tst_check(zox_tst_xr_options_slid, "the stick didn't step the slider (or its text)");
    zox_tst_check(zox_tst_xr_options_scrolled, "the list didn't scroll as it was navigated down");
    zox_tst_check(!zox_tst_xr_options_hidden, "a selected row stayed hidden (the list didn't scroll to it)");
    zox_tst_check(zox_tst_xr_options_handle_end, "scrolled to its end, the scrollbar's handle isn't at its bottom");
    zox_tst_check(zox_tst_xr_options_back, "Back didn't return to the main menu");
}

void zox_tsts_xr_options(ecs* world) {
    zox_tst_add("xr_options", zox_tst_xr_options_ready, zox_tst_xr_options_check, 120);
    zox_tst_set_setup("xr_options", zox_tst_xr_sim_setup, 1);
}
