// tst: xr_options, the options menu on 3D huds by the XR controllers alone, on the simulator: the 2D options window
//  (huds/ins/settings.c, tabs of sliders and toggles) built in 3D by the same code (its 3D twins)
//  - A on the start menu, the stick down to Options on the main menu, A: the options window is 3D (a Canvas3)
//  - the stick down to a slider, then right: its value steps up (or stays at its end), its text shows it
//    (navigation_slider_system)
//  - the stick down to a toggle, A: its setting flips
//  - RB: the next tab shown, the selection on its button; LB: back (navigation_tabs_system)
//  - B: the window's close button, the main menu again (cancel_menu_system)
//  - a selection never hidden long (a tab's hidden elements, or scrolled out of its list)
//  - run: bash bsh/tst.sh xr_options (screenshot: the window, a slider selected, xr_options.ppm)
int zox_tst_xr_options_stage = 0;
int zox_tst_xr_options_frames = 0;
byte zox_tst_xr_options_menu3 = 0;
entity zox_tst_xr_options_setting = 0;
float zox_tst_xr_options_before = 0;
byte zox_tst_xr_options_slid = 0;           // the stick stepped a slider, its text with it
byte zox_tst_xr_options_toggled = 0;        // A flipped a toggle's setting
byte zox_tst_xr_options_tab_next = 0;       // RB showed the next tab
byte zox_tst_xr_options_tab_back = 0;       // LB the one before
byte zox_tst_xr_options_back = 0;
byte zox_tst_xr_options_hidden = 0;         // a selected element hidden a while
int zox_tst_xr_options_hidden_frames = 0;
int zox_tst_xr_options_tab = -1;

static inline void zox_tst_xr_options_next() {
    zox_tst_xr_options_stage++;
    zox_tst_xr_options_frames = 0;
}

static inline entity zox_tst_xr_options_target(ecs* world) {
    const entity right = xr_controllers[xr_hand_right];
    return zox_valid(right) ? zox_getv(right, Target) : 0;
}

static inline float zox_tst_xr_options_value(ecs* world, entity setting) {
    if (zox_has(setting, SettingByte)) {
        return zox_getv(setting, SettingByte);
    }
    return zox_has(setting, SettingFloat) ? zox_getv(setting, SettingFloat) : 0;
}

// a slider's text is its setting's name and value (on_settings_slide)
static inline byte zox_tst_xr_options_text_matches(ecs* world, entity slider, entity setting) {
    const entity text = zox_get_child_by_id(world, slider, zox_id(Text));
    if (!zox_valid(text)) {
        return 0;
    }
    char label[128];
    const char* name = zox_get(setting, ZoxName)->value;
    if (zox_has(setting, SettingFloat)) {
        snprintf(label, sizeof(label), "%s [%.2f]", name, zox_getv(setting, SettingFloat));
    } else {
        snprintf(label, sizeof(label), "%s [%i]", name, zox_getv(setting, SettingByte));
    }
    return is_zext((TextData*) zox_get(text, TextData), label);
}

// the shown tab in the selection's window (its active TabButton's ChildIndex), -1 if none
static inline int zox_tst_xr_options_shown_tab(ecs* world, entity canvas) {
    const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuOptions));
    if (!zox_valid(menu)) {
        return -1;
    }
    entity tabs[16];
    const uint count = zox_get_children_by_id_recursive(world, menu, tabs, 16, zox_id(TabButton), 0);
    for (uint k = 0; k < count; k++) {
        if (zox_getv(tabs[k], ActiveState)) {
            return zox_getv(tabs[k], ChildIndex);
        }
    }
    return -1;
}

// the stick down a moment (the selection one down), then let go, every 16 frames
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
    xr_sim_script_b = 0;
    xr_sim_script_lb = 0;
    xr_sim_script_rb = 0;
    if (zox_tst_xr_options_stage >= 2 && zox_tst_xr_options_stage < 8 && zox_valid(target) && zox_has(target, RenderDisabled)) {
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
            zox_tst_waiting = "a slider selected";
            if (f > 20 && zox_valid(target) && zox_has(target, SliderLabel) && zox_has(target, SettingLink)) {
                xr_sim_script_stick = float2_zero;
                zox_tst_xr_options_setting = zox_getv(target, SettingLink);
                zox_tst_xr_options_before = zox_tst_xr_options_value(world, zox_tst_xr_options_setting);
                snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("xr_options.ppm"));
                zox_tst_xr_options_next();
                return 0;
            }
            // the list's end (a tab with no sliders, Video's toggles): RB to the next tab
            if (f % 96 >= 80 && f % 96 < 84) {
                xr_sim_script_stick = float2_zero;
                xr_sim_script_rb = 1;
                return 0;
            }
            zox_tst_xr_options_down(f);
            return f > 1200;
        case 3:
            zox_tst_waiting = "the stick right on it";
            xr_sim_script_stick = f >= 4 && f < 8 ? (float2) { 1, 0 } : float2_zero;
            if (f > 24) {
                const entity setting = zox_tst_xr_options_setting;
                const float now = zox_tst_xr_options_value(world, setting);
                const float end = zox_has(setting, SettingFloat) ? zox_getv(setting, SettingFloatBounds).y :
                    zox_getv(setting, SettingByteBounds).y;
                const float start = zox_has(setting, SettingFloat) ? zox_getv(setting, SettingFloatBounds).x :
                    zox_getv(setting, SettingByteBounds).x;
                // one push, one step (navigation_slider_repeat): not run to its end
                const float most = zox_has(setting, SettingFloat) ? (end - start) / 20 + 0.001f : 1;
                zox_tst_xr_options_slid = (now > zox_tst_xr_options_before || now >= end - 0.0001f) &&
                    now - zox_tst_xr_options_before <= most &&
                    zox_tst_xr_options_text_matches(world, target, setting);
                zox_log("[tst] xr_options: the slider [%s] from [%.3f] to [%.3f]", zox_get(setting, ZoxName)->value,
                    zox_tst_xr_options_before, now);
                zox_tst_xr_options_next();
            }
            return 0;
        case 4:
            zox_tst_waiting = "a toggle selected";
            if (zox_valid(target) && zox_has(target, Toggle) && zox_has(target, SettingLink) && f > 10) {
                xr_sim_script_stick = float2_zero;
                zox_tst_xr_options_setting = zox_getv(target, SettingLink);
                zox_tst_xr_options_before = zox_tst_xr_options_value(world, zox_tst_xr_options_setting);
                zox_tst_xr_options_next();
                return 0;
            }
            zox_tst_xr_options_down(f);
            return f > 900;
        case 5:
            zox_tst_waiting = "A on the toggle";
            xr_sim_script_a = f >= 4 && f < 8;
            if (f > 24) {
                zox_tst_xr_options_toggled = zox_tst_xr_options_value(world, zox_tst_xr_options_setting) !=
                    zox_tst_xr_options_before;
                zox_tst_xr_options_tab = zox_tst_xr_options_shown_tab(world, canvas);
                zox_tst_xr_options_next();
            }
            return 0;
        case 6:
            zox_tst_waiting = "RB to the next tab";
            xr_sim_script_rb = f >= 4 && f < 8;
            if (f > 24) {
                const int tab = zox_tst_xr_options_shown_tab(world, canvas);
                zox_tst_xr_options_tab_next = tab >= 0 && tab != zox_tst_xr_options_tab && zox_valid(target) &&
                    zox_has(target, TabButton);
                zox_log("[tst] xr_options: RB, the tab [%i] to [%i]", zox_tst_xr_options_tab, tab);
                zox_tst_xr_options_next();
            }
            return 0;
        case 7:
            zox_tst_waiting = "LB to the tab before";
            xr_sim_script_lb = f >= 4 && f < 8;
            if (f > 24) {
                zox_tst_xr_options_tab_back = zox_tst_xr_options_shown_tab(world, canvas) == zox_tst_xr_options_tab;
                zox_tst_xr_options_next();
            }
            return 0;
        case 8:
            zox_tst_waiting = "B back to the main menu";
            xr_sim_script_b = f >= 4 && f < 8;
            if (zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuMain)))) {
                zox_tst_xr_options_back = 1;
                return 1;
            }
            return f > 120;
    }
    return 0;
}

void zox_tst_xr_options_check(ecs* world) {
    (void) world;
    zox_log("[tst] xr_options: 3D [%i], the stick a slider [%i], A a toggle [%i], RB [%i], LB [%i], a hidden selection [%i], back [%i]",
        zox_tst_xr_options_menu3, zox_tst_xr_options_slid, zox_tst_xr_options_toggled, zox_tst_xr_options_tab_next,
        zox_tst_xr_options_tab_back, zox_tst_xr_options_hidden, zox_tst_xr_options_back);
    zox_tst_check(zox_tst_xr_options_menu3, "the options window isn't 3D");
    zox_tst_check(zox_tst_xr_options_slid, "the stick didn't step the slider (or its text)");
    zox_tst_check(zox_tst_xr_options_toggled, "A didn't flip the toggle's setting");
    zox_tst_check(zox_tst_xr_options_tab_next, "RB didn't show the next tab (its button selected)");
    zox_tst_check(zox_tst_xr_options_tab_back, "LB didn't show the tab before");
    zox_tst_check(!zox_tst_xr_options_hidden, "a selected element stayed hidden");
    zox_tst_check(zox_tst_xr_options_back, "B didn't return to the main menu");
}

void zox_tsts_xr_options(ecs* world) {
    zox_tst_add("xr_options", zox_tst_xr_options_ready, zox_tst_xr_options_check, 120);
    zox_tst_set_setup("xr_options", zox_tst_xr_sim_setup, 1);
}
