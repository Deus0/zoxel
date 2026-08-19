#ifndef zoxm_time
#define zoxm_time

// 15 seconds of recording
#define record_frames_ 15
#define record_frames_count 60 * record_frames_

byte zox_log_lags = 0;
entity max_systems_data = 0;
entity frame_times_samples = 0;
double zox_delta_time_system = 0;
uint system_times_display_count = 20;
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
    define_systems_timing_debug(world);
    zox_import_module(Timers);
    // this for profiler data
    {
        frame_times_samples = zox_new();
        DoubleData data = (DoubleData) { 0 };
        initialize_DoubleData(&data, record_frames_count);
        zox_set_ptr(frame_times_samples, DoubleData, data);
    }
    // stats
#ifdef zox_time_systems
    add_hook_spawn_prefabs(add_system_log_components);
    {
        max_systems_data = zox_new();
        zox_add(max_systems_data, TrackMaxSystem);
        zox_add(max_systems_data, SystemLink);
    }
#endif
} zox_end_module(Timing);

#endif
