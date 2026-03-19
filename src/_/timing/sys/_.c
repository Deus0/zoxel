#include "time_destroy.c"
#include "timers.c"
#include "destroy_in_frame.c"
#include "timed_event.c"

void define_systems_timing(ecs* world) {
    zox_system(
        TimerSystem,
        EcsOnUpdate,
        [in] TimerRate,
        [out] TimerState,
        [out] TimerTime
    );
    zox_system( // _1
        DestroyInTimeSystem,
        zoxp_destroy,
        [out] timing.DestroyInTime
    );
    zox_system(
        DestroyInFrameSystem,
        EcsOnStore,
        [none] core.DestroyInFrame
    );
    zox_system_1(
        TimedEventSystem,
        EcsPreStore,
        [in] core.TimedEvent,
        [in] core.EventInput,
        [out] core.EventTime
    );
}