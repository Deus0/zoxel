#include "warmup.c"
zox_increment_system_with_reset(Activate, zox_dirty_end);
zox_increment_system_with_reset(WarmupState, zox_dirty_end);
zox_increment_system_with_reset(CooldownState, zox_dirty_end);

void define_systems_users(ecs* world) {
    zoxd_system_increment(Activate);
    zoxd_system_increment(WarmupState);
    zoxd_system_increment(CooldownState);
    zox_system(
        WarmupSystem,
        EcsOnUpdate,
        [in] users.Activate,
        [out] users.Warmup
    );
    zox_system(
        WarmupStateSystem,
        EcsOnUpdate,
        [in] users.WarmupTime,
        [out] users.Warmup,
        [out] users.WarmupState
    );
}