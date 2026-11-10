// Smooth limb swinging
//  - phase is accumulated per bone, so changing speed never jumps the angle
//  - SwingBlend eases between idle and walking, blending speed and angle
//  - the bone slerps towards its target, smoothing any other snaps (i.e. shoulder raise ending)
void simple_swing_system(iter* it, byte limb) {
    const float idle_speed = 0.1f;      // half cycles per second
    const float idle_angle = 12;        // degrees
    const float walk_speed = 1.8f;
    const float blend_rate = 5.0f;      // how fast idle <-> walk eases
    const float follow_rate = 18.0f;    // how fast the bone slerps to its target
    const float max_delta = 0.1f;
    float delta = zox_delta_time;
    if (delta > max_delta) {
        delta = max_delta;
    }
    const float blend_t = 1.0f - expf(-blend_rate * delta);
    const float follow_t = 1.0f - expf(-follow_rate * delta);
    zox_sys_begin();
    zox_sys_in(SwingState);
    zox_sys_in(SwingAngle);
    zox_sys_in(LocalPosition3D);
    zox_sys_out(SwingPhase);
    zox_sys_out(SwingBlend);
    zox_sys_out(LocalRotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SwingState, state);
        zox_sys_i(SwingAngle, swing_angle);
        zox_sys_i(LocalPosition3D, position);
        zox_sys_o(SwingPhase, phase);
        zox_sys_o(SwingBlend, blend);
        zox_sys_o(LocalRotation3D, rotation);
        if (!state->value) {
            continue;
        }
        // ease between idle and walking
        const float target_blend = state->value == 2 ? 1.0f : 0.0f;
        blend->value += (target_blend - blend->value) * blend_t;
        const float speed = idle_speed + (walk_speed - idle_speed) * blend->value;
        const float angle = idle_angle + ((float) swing_angle->value - idle_angle) * blend->value;
        // advance phase, wrapped to keep precision
        phase->value += M_PI * speed * delta;
        if (phase->value > 2 * M_PI) {
            phase->value -= 2 * M_PI;
        }
        float swing = sinf(phase->value);
        // NOTE: Opposite sides swing opposite ways
        if (position->value.x < 0) {
            swing = -swing;
        }
        // NOTE: Reverse direction for legs
        if (limb) {
            swing = -swing;
        }
        const float4 target = euler_to_quaternion((float3) {
            swing * angle * degrees_to_radians,
            0,
            0 });
        rotation->value = quaternion_normalize2(
            quaternion_slerp(rotation->value, target, follow_t));
    }
}

zox_sys2(ArmSwingSystem) {
    simple_swing_system(it, 0);
} zox_sys_end(ArmSwingSystem);

zox_sys2(LegSwingSystem) {
    simple_swing_system(it, 1);
} zox_sys_end(LegSwingSystem);
