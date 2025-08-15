// #include "printer.c"
#include "lines.c"

void key_down_toggle_light_debug(ecs *world, int32_t keycode) {
    (void) world;
    if (keycode == SDLK_F7) {
        zox_debug_lights = !zox_debug_lights;
        zox_log("+ Debug Lights [%s]", zox_debug_lights ? "Enabled" : "Disabled");
    }
}