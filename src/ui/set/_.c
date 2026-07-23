// TODO: Scale UI based on screen size
byte ui_scale = 4;
byte keyboard_navigation_mode = 0;
color active_outline = (color) { 100, 222, 150, 155 };
#include "animations.c"
#include "buttons.c"
#include "icons.c"
#include "input.c"
#include "selection.c"
#include "windows.c"

// Use this to set ui_scale dynamically
byte calculate_ui_scale(int2 size) {
    int area = size.x * size.y;
    if (area <= 512 * 512)   return 1;
    if (area <= 1024 * 1024) return 2;
    if (area <= 2048 * 2048) return 3;
    return 4;
}
void initialize_settings_elements(ecs* world) {
    /*if (is_on_phosh()) {
        zox_log("Phosh Detected. Small UI Enabled.");
        ui_scale = 2;
    }*/
    int2 screen_size = get_screen_size();
    byte new_scale = calculate_ui_scale(screen_size);
    ui_scale = new_scale;
    zox_log("Screen [%ix%i] - UI Scale set to: %i", screen_size.x, screen_size.y, new_scale);
}

void key_down_toggle_keyboard_navigation(ecs *world, int32_t keycode) {
    if (keycode == zox_key_TAB) {
        keyboard_navigation_mode = !keyboard_navigation_mode;
        zox_log("Toggled Navigation [%s]", keyboard_navigation_mode ? "On" : "Off");
    }
}
