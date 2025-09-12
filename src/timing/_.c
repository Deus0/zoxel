#ifndef zoxm_time
#define zoxm_time

#include "dat/settings.c"
zoxc_double(DestroyInTime);
zoxc_double(TimerTime);
zoxc_double(TimerRate);
zoxc_byte(TimerState);
#include "mcr/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Timing)
    zoxd_double(DestroyInTime);
    zoxd_double(TimerTime);
    zoxd_double(TimerRate);
    zoxd_byte(TimerState);
    define_systems_timing(world);
    initialize_time();
    add_to_post_update_loop(iterate_time);
    add_to_post_update_loop(iterate_time_system);
zox_end_module(Timing)

#endif