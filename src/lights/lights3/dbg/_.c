// #include "printer.c"
#include "lines.c"
// #include "sides.c"

void key_down_toggle_light_debug(ecs *world, int32_t keycode) {
    (void) world;
    if (keycode == zox_key_F7) {
        // zox_debug_lights = !zox_debug_lights;
        zox_debug_lights++;
        if (zox_debug_lights == terrain_depth + 2) zox_debug_lights = 0;
        zox_log("+ Debug Lights [%s] Depth [%i]",
            zox_debug_lights ? "Enabled" : "Disabled",
            zox_debug_lights ? (zox_debug_lights - 1) : 0);
    }
}
