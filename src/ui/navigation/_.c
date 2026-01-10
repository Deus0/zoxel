#ifndef zox_elements_navigation
#define zox_elements_navigation

// TODO: We should handle navigator selection differently so it properly  removes/adds when device mode changes

#include "sys/_.c"

zox_begin_module(Navigation)
    define_systems_navigation(world);
zox_end_module(Navigation)

#endif