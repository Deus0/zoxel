#include "warmup.c"
#include "cooldown.c"

void define_systems_users(ecs* world) {
    zox_system(
        WarmupSystem,
        EcsOnUpdate,
        [in] users.Activate,
        [in] users.CooldownAt,
        [out] users.WarmupAt
    );
    zox_system(
        WarmupStateSystem,
        EcsOnUpdate,
        [in] users.WarmupTime,
        [out] users.WarmupAt,
        [out] users.WarmupState
    );
    zox_system(
        CooldownSystem,
        EcsOnUpdate,
        [in] users.WarmupState,
        [out] users.CooldownAt
    );
    zox_system(
        CooldownStateSystem,
        EcsOnUpdate,
        [in] users.CooldownTime,
        [out] users.CooldownAt,
        [out] users.CooldownState
    );
}