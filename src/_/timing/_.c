#ifndef zoxm_time
#define zoxm_time

#include "set/_.c"
#include "com/_.c"
#include "mcr/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"
#include "timers/_.c"

zox_begin_module(Timing) {
    define_components_timing(world);
    define_systems_timing(world);
    initialize_time();
    add_to_post_update_loop(iterate_time);
    add_to_post_update_loop(iterate_time_system);
    // stats
    add_hook_spawn_prefabs(add_system_log_components);
    // add_to_post_update_loop(log_lagging_systems);
    define_systems_timing_debug(world);
    zox_import_module(Timers);
    // Add debugging to this module
    entity module = zox_id(Timing);
    zox_add_tag(module, TrackMaxSystem);
    zox_add(module, SystemLink);
} zox_end_module(Timing);

#endif
