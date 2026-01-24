#ifndef zoxm_time
#define zoxm_time

#include "dat/settings.c"
#include "com/_.c"
#include "mcr/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(Timing)
    define_components_timing(world);
    define_systems_timing(world);
    initialize_time();
    add_to_post_update_loop(iterate_time);
    add_to_post_update_loop(iterate_time_system);
    // stats
    add_hook_spawn_prefabs(add_system_log_components);
    // add_to_post_update_loop(log_lagging_systems);

    define_systems_timing_debug(world);
zox_end_module(Timing)

#endif