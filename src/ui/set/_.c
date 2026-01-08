// TODO: Scale UI based on screen size
byte ui_scale = 4;

#include "animations.c"
#include "buttons.c"
#include "icons.c"
#include "input.c"
#include "selection.c"
#include "windows.c"

void initialize_settings_elements(ecs* world) {
    if (is_on_phosh()) {
        zox_log("Phosh Detected. Small UI Enabled.");
        ui_scale = 2;
    }
}