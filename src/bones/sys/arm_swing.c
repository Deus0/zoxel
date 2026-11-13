// Smooth limb swinging, in step: every limb reads its skeleton's SwingPhase and SwingBlend (arm_control.c)
//  - SwingBlend eases between idle and walking, blending the angle
//  - the bone slerps towards its target, smoothing any other snaps (i.e. shoulder raise ending)
void simple_swing_system(iter* it, byte limb) {
    const float idle_angle = 12;        // degrees
    const float follow_rate = 18.0f;    // how fast the bone slerps to its target
    const float delta = zox_delta_time < swing_max_delta ? zox_delta_time : swing_max_delta;
    const float follow_t = 1.0f - expf(-follow_rate * delta);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SwingState);
    zox_sys_in(SwingAngle);
    zox_sys_in(LocalPosition3D);
    zox_sys_out(LocalRotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SwingState, state);
        zox_sys_i(SwingAngle, swing_angle);
        zox_sys_i(LocalPosition3D, position);
        zox_sys_o(LocalRotation3D, rotation);
        if (!state->value) {
            continue;
        }
        const entity skeleton = zox_get_link(world, e, SkeletonLink);
        if (!zox_valid(skeleton) || !zox_has(skeleton, SwingPhase)) {
            continue;
        }
        const float blend = zox_getv(skeleton, SwingBlend);
        const float angle = idle_angle + ((float) swing_angle->value - idle_angle) * blend;
        float swing = sinf(zox_getv(skeleton, SwingPhase));
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

void arm_swing_system(iter* it) {
    zox_sys_on_begin();
    simple_swing_system(it, 0);
    zox_sys_on_end();
} zoxd_system(arm_swing_system);

void leg_swing_system(iter* it) {
    zox_sys_on_begin();
    simple_swing_system(it, 1);
    zox_sys_on_end();
} zoxd_system(leg_swing_system);
