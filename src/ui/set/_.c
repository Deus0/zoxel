// TODO: Scale UI based on screen size
byte ui_scale = 4;
byte keyboard_navigation_mode = 0;

#include "animations.c"
#include "buttons.c"
#include "icons.c"
#include "input.c"
#include "selection.c"
#include "windows.c"

void initialize_settings_elements(ecs* world) {
    /*if (is_on_phosh()) {
        zox_log("Phosh Detected. Small UI Enabled.");
        ui_scale = 2;
    }*/
    int2 screen_size = get_screen_size();
    byte new_scale = calculate_ui_scale(screen_size);
    zox_logv("Screen [%ix%i] - UI Scale set to: %i", screen_size.x, screen_size.y, new_scale);
    ui_scale = new_scale;
}

void key_down_toggle_keyboard_navigation(ecs *world, int32_t keycode) {
    if (keycode == SDLK_TAB) {
        keyboard_navigation_mode = !keyboard_navigation_mode;
        zox_log("Toggled Navigation [%s]", keyboard_navigation_mode ? "On" : "Off");
    }
}