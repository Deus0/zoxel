// TODO: Scale UI based on screen size
float ui_scale = 1;
byte keyboard_navigation_mode = 0;
color active_outline = (color) { 100, 222, 150, 155 };
#include "animations.c"
#include "buttons.c"
#include "icons.c"
#include "input.c"
#include "selection.c"
#include "windows.c"

float calculate_ui_scale(int2 size) {
    float area = (float) size.x * (float) size.y;
    float scale = sqrtf(area / (512 * 256));
    if (scale < 0.25f) scale = 0.25f;
    if (scale > 4.0f) scale = 4.0f;
    return scale;
}

void initialize_settings_elements(ecs* world) {
    /*if (is_on_phosh()) {
        zox_log("Phosh Detected. Small UI Enabled.");
        ui_scale = 2;
    }*/
    int2 screen_size = get_screen_size();
    float new_scale = calculate_ui_scale(screen_size);
    ui_scale = new_scale;
    // zox_log("Screen [%ix%i] - UI Scale set to: %i", screen_size.x, screen_size.y, new_scale);
}

void key_down_toggle_keyboard_navigation(
    ecs *world,
    int32_t keycode)
{
    if (keycode == zox_key_TAB) {
        keyboard_navigation_mode = !keyboard_navigation_mode;
        zox_log("Toggled Navigation [%s]", keyboard_navigation_mode ? "On" : "Off");
    }
}
