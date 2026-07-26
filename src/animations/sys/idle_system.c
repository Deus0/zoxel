// a simple idle scale animation
zox_sys2(IdleSystem) {
    const float idle_default_scale = 1.0f;
    const float idle_scale_addition = 0.02f;
    const float idle_animation_speed = 4.0f;
    double time = zox_current_time;
    zox_sys_begin();
    zox_sys_in(AnimationState);
    zox_sys_in(AnimationStart);
    zox_sys_out(Scale1);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(AnimationState, type);
        zox_sys_i(AnimationStart, start);
        zox_sys_o(Scale1, scale);
        if (type->value != zox_animation_idle) {
            continue;
        }
        double animation_time = time - start->value;
        scale->value = idle_default_scale + idle_scale_addition * sin(animation_time * idle_animation_speed);
    }
} zox_sys_end(IdleSystem);