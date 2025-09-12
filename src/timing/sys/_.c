#include "time_destroy.c"
#include "timers.c"

void define_systems_timing(ecs* world) {
    zox_system(TimerSystem, EcsOnUpdate,
        [in] TimerRate,
        [out] TimerState,
        [out] TimerTime);
    zox_system_1(DestroyInTimeSystem, zoxp_destroy,
        [out] timing.DestroyInTime);
}