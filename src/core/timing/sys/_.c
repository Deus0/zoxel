#include "time_destroy.c"
#include "timers.c"
#include "destroy_in_frame.c"
#include "timed_event.c"
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
        DestroyInTimeSystem,
        zoxp_destroy,
        [out] timing.DestroyInTime
    );
    zox_system(
        DestroyInFrameSystem,
        zoxp_destroy,
        [none] core.DestroyInFrame
    );
    zox_system_1(
        TimedEventSystem,
        zoxp_spawn,
        [in] core.TimedEvent,
        [in] core.EventInput,
        [out] core.EventTime
    );
    zox_system(
        FpsCurveSystem,
        zoxp_update,
        0
    );
    zox_system_1(
        iterate_time_system,
        zoxp_rendering_end,
        0
    );
}
