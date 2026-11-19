// Legs reach their feet to IKTarget by IKWeight (two bone ik: the thigh, the calf the knee, the foot the end)
//  - lengths from the bones' world positions (one frame's), directions at rest from their local positions
//  - the knee bends ahead (the body's +z), the foot kept level with the body's facing (flat on the ground)
//  - the weight blends over the walk's swing (the thigh's); at 0 the calf and foot ease back straight, the calf left
//    and foot left to the walk while it swings the leg (it bends the knee, levels the foot crouching)
//  - zoxp_layouts as arm_ik: after the drivers (sys/feet.c, zoxp_trails), before the hierarchy, from the bones
//    carried to where the body is now (skeleton_now)
#define ik_knee_return_rate 18.0f

void leg_ik_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const float delta = zox_delta_time < 0.1f ? zox_delta_time : 0.1f;
    const float return_t = 1.0f - expf(-ik_knee_return_rate * delta);
    zox_sys_begin();
    zox_sys_in(IKTarget);
    zox_sys_in(IKWeight);
    zox_sys_in(Position3D);
    zox_sys_out(LocalRotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(IKTarget, target);
        zox_sys_i(IKWeight, weight);
        zox_sys_i(Position3D, position);
        zox_sys_o(LocalRotation3D, rotation);
        ik_leg leg;
        if (!ik_leg_find(world, e, &leg)) {
            continue;
        }
        LocalRotation3D* calf_rotation = zox_getm(leg.calf, LocalRotation3D);
        LocalRotation3D* foot_rotation = zox_getm(leg.foot, LocalRotation3D);
        const float w = weight->value < 0 ? 0 : (weight->value > 1 ? 1 : weight->value);
        if (w <= 0.001f) {
            // straight again, unless the walk swings this leg (it bends the knee, sys/arm_swing.c)
            if (!(zox_has(e, SwingState) && zox_getv(e, SwingState)) && !float4_equals(calf_rotation->value, quaternion_identity)) {
                calf_rotation->value = quaternion_normalize2(quaternion_slerp(calf_rotation->value, quaternion_identity, return_t));
            }
            if (!(zox_has(e, SwingState) && zox_getv(e, SwingState)) && !float4_equals(foot_rotation->value, quaternion_identity)) {
                foot_rotation->value = quaternion_normalize2(quaternion_slerp(foot_rotation->value, quaternion_identity, return_t));
            }
            continue;
        }
        const entity skeleton = zox_get_link(world, e, SkeletonLink);
        const skeleton_now now = skeleton_now_get(world, skeleton);
        const entity hips = zox_get_parent(world, e);
        const float4 hips_rotation = skeleton_now_rotation(&now, zox_valid(hips) && zox_has(hips, Rotation3D) ?
            zox_getv(hips, Rotation3D) : quaternion_identity);
        const float3 thigh_point = skeleton_now_point(&now, position->value);
        const float a = float3_distance(position->value, zox_getv(leg.calf, Position3D));
        const float b = float3_distance(zox_getv(leg.calf, Position3D), zox_getv(leg.foot, Position3D));
        const float3 upper_rest = zox_getv(leg.calf, LocalPosition3D);
        const float3 lower_rest = zox_getv(leg.foot, LocalPosition3D);
        if (a < 0.0001f || b < 0.0001f || float3_length(upper_rest) < 0.0001f || float3_length(lower_rest) < 0.0001f) {
            continue;
        }
        const float3 pole = float4_rotate_float3(now.now_rotation, (float3) { 0, 0, 1 });    // the knee ahead
        float3 knee_point, foot_point;
        ik_two_bone(thigh_point, a, b, target->value, pole, &knee_point, &foot_point);
        const float4 thigh_ik = quaternion_from_to(upper_rest,
            float4_rotate_float3(quaternion_inverse(hips_rotation), float3_sub(knee_point, thigh_point)));
        const float4 calf_parent = quaternion_rotate(hips_rotation, thigh_ik);
        const float4 calf_ik = quaternion_from_to(lower_rest,
            float4_rotate_float3(quaternion_inverse(calf_parent), float3_sub(foot_point, knee_point)));
        rotation->value = quaternion_normalize2(quaternion_slerp(rotation->value, thigh_ik, w));
        calf_rotation->value = quaternion_normalize2(quaternion_slerp(quaternion_identity, calf_ik, w));
        // the foot level: its world turn the body's
        const float4 foot_ik = quaternion_rotate(quaternion_inverse(quaternion_rotate(calf_parent, calf_ik)), now.now_rotation);
        foot_rotation->value = quaternion_normalize2(quaternion_slerp(quaternion_identity, foot_ik, w));
    }
    zox_sys_on_end();
} zoxd_system(leg_ik_system);
