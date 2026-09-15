zox_sys2(DeathAnimationSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(AnimationState);
    zox_sys_out(AnimationStart);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(AnimationState, state);
        zox_sys_o(AnimationStart, start);
        if (start->value) {
            continue;
        }
        state->value = zox_animation_shrink;
        start->value = zox_current_time;
    }
} zox_sys_end(DeathAnimationSystem);
