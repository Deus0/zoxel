
// NOTE: Keyboard navigation now broken

// TODO: We should handle navigator selection differently so it properly  removes/adds when device mode changes

#include "sys/_.c"

void import_navigation(ecs* world) {
    zox_module(navigation);
    zox_systems_navigation(world);
}