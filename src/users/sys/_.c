#include "warmup.c"
#include "cooldown.c"
zox_increment_system_with_reset(Activate, zox_dirty_end);
zox_increment_system_with_reset(WarmupState, zox_dirty_end);
zox_increment_system_with_reset(CooldownState, zox_dirty_end);
zox_increment_system_with_reset(UserDataDirty, zox_dirty_end);

void define_systems_users(ecs* world) {
    zoxd_system_increment(Activate);
    zoxd_system_increment(WarmupState);
    zoxd_system_increment(CooldownState);
    zoxd_system_increment(UserDataDirty);

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