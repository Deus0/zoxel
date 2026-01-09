#include "time_destroy.c"
#include "timers.c"

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
    // TODO: Move this before other systems
    zox_system(
        SystemDeltaLogResetSystem,
        EcsOnLoad,
        [out] timing.SystemDelta
    );
    zox_system(
        SystemDeltaLogSystem,
        EcsOnStore,
        [in] timing.SystemDelta
    );
}