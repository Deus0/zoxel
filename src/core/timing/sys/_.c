#include "time_destroy.c"
#include "timers.c"
#include "destroy_in_frame.c"
#include "system_time.c"

void zox_systems_timing(ecs* world) {
    zox_system(
        timer_system,
        zoxp_update,
        [in] TimerRate,
        [out] TimerState,
        [out] TimerTime
    );
    zox_system(
        destroy_in_time_system,
        zoxp_destroy,
        [out] timing.DestroyInTime
    );
    zox_system(
        destroy_in_frame_system,
        zoxp_destroy,
        [none] core.DestroyInFrame
    );
    zox_system(
        fps_curve_system,
        zoxp_update,
        0
    );
    zox_system_1(
        iterate_time_system,
        zoxp_time,
        0
    );
}
