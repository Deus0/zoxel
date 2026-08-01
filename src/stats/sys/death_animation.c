zox_sys2(DeathAnimationSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Dead);
    zox_sys_out(DiedTime);
    zox_sys_out(AnimationState);
    zox_sys_out(AnimationStart);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Dead, dead);
        zox_sys_o(DiedTime, diedTime);
        zox_sys_o(AnimationState, animationState);
        zox_sys_o(AnimationStart, animationStart);
        if (dead->value != zox_dirty_active) {
            continue;
        }
        diedTime->value = zox_current_time;
        animationState->value = zox_animation_shrink;
        animationStart->value = zox_current_time;
        zox_add(e, DisableMovement);
    }
} zox_sys_end(DeathAnimationSystem);