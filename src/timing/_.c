#ifndef zoxm_time
#define zoxm_time

#include "data/settings.c"
zoxc_double(DestroyInTime);
zoxc_double(TimerTime);
zoxc_double(TimerRate);
zoxc_byte(TimerState);
#include "macros/_.c"
#include "util/_.c"
#include "systems/_.c"

zox_begin_module(Timing)
    zox_define_component_double(DestroyInTime);
    zox_define_component_double(TimerTime);
    zox_define_component_double(TimerRate);
    zoxd_byte(TimerState);
    define_systems_timing(world);
    initialize_time();
    add_to_post_update_loop(iterate_time);
    add_to_post_update_loop(iterate_time_system);
zox_end_module(Timing)

#endif