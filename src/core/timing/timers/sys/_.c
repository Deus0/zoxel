#include "warmup.c"
#include "activate.c"
#include "cooldown.c"

void zox_define_systems_timers(ecs* world) {
    zox_system(
        WarmupSystem,
        zoxp_update,
        [in] timers.ActivateBegin,
        [in] timers.CooldownAt,
        [out] timers.WarmupAt
    );
    zox_system(
        WarmupStateSystem,
        zoxp_update,
        [in] timers.WarmupTime,
        [out] timers.WarmupAt,
        [out] timers.WarmupState
    );
    zox_system(
        ActivateSystem,
        zoxp_update,
        [in] timers.WarmupState,
        [out] timers.Activate
    );
    zox_system(
        CooldownSystem,
        zoxp_update,
        [in] timers.WarmupState,
        [out] timers.CooldownAt
    );
    zox_system(
        CooldownStateSystem,
        zoxp_update,
        [in] timers.CooldownTime,
        [out] timers.CooldownAt,
        [out] timers.CooldownState
    );
}
