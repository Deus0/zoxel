// #include "printer.c"
#include "lines.c"
// #include "sides.c"

void zox_dbg_cycle_light_debug(ecs *world, ClickEventData data) {
    // zox_debug_lights = !zox_debug_lights;
    zox_debug_lights++;
    if (zox_debug_lights > terrain_depth + 1) {
        zox_debug_lights = 0;
    }
    if (!zox_debug_lights) {
        zox_log("Debug Lights Disabled");
    } else {
        zox_log("+ Debug Lights Depth [%i]", (zox_debug_lights - 1));
    }
}
