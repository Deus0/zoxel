#include "warmup.c"
#include "activate.c"
#include "cooldown.c"

void zox_define_systems_timers(ecs* world) {
    zox_system(
        WarmupSystem,
        EcsOnUpdate,
        [in] timers.ActivateBegin,
        [in] timers.CooldownAt,
        [out] timers.WarmupAt
    );
    zox_system(
        WarmupStateSystem,
        EcsOnUpdate,
        [in] timers.WarmupTime,
        [out] timers.WarmupAt,
        [out] timers.WarmupState
    );
    zox_system(
        ActivateSystem,
        EcsOnUpdate,
        [in] timers.WarmupState,
        [out] timers.Activate
    );
    zox_system(
        CooldownSystem,
        EcsOnUpdate,
        [in] timers.WarmupState,
        [out] timers.CooldownAt
    );
    zox_system(
        CooldownStateSystem,
        EcsOnUpdate,
        [in] timers.CooldownTime,
        [out] timers.CooldownAt,
        [out] timers.CooldownState
    );
}
