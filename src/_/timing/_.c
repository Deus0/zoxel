#ifndef zoxm_time
#define zoxm_time

// zoxd_module(Timing);
entity timing_module;
entity frame_times_samples;
double zox_delta_time_system = 0;
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
    // stats
    add_hook_spawn_prefabs(add_system_log_components);
    define_systems_timing_debug(world);
    zox_import_module(Timers);
    // Add debugging to this module
    entity module = zox_id(Timing);
    zox_add_tag(module, TrackMaxSystem);
    zox_add(module, SystemLink);
    timing_module = module;
    {
        frame_times_samples = zox_new();
        DoubleData data = (DoubleData) { 0 };
        initialize_DoubleData(&data, record_frames_count);
        zox_set_ptr(frame_times_samples, DoubleData, data);
    }
} zox_end_module(Timing);

#endif
