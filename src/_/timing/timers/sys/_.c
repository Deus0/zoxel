#include "warmup.c"
#include "activate.c"
#include "cooldown.c"

void zox_systems_timers(ecs* world) {
    zox_system(
        warmup_system,
        zoxp_update,
        [in] timers.ActivateBegin,
        [in] timers.CooldownAt,
        [out] timers.WarmupAt
    );
    zox_system(
        warmup_state_system,
        zoxp_update,
        [in] timers.WarmupTime,
        [out] timers.WarmupAt,
        [out] timers.WarmupState
    );
    zox_system(
        activate_system,
        zoxp_update,
        [in] timers.WarmupState,
        [out] timers.Activate
    );
    zox_system(
        cooldown_system,
        zoxp_update,
        [in] timers.WarmupState,
        [out] timers.CooldownAt
    );
    zox_system(
        cooldown_state_system,
        zoxp_update,
        [in] timers.CooldownTime,
        [out] timers.CooldownAt,
        [out] timers.CooldownState
    );
}
