// Bends a shoulder's arm to its IKTarget, by IKWeight, over its animation (swing, raise)
//  - the shoulder turns the upper arm (the shoulder to the elbow), the elbow (the hand's parent) the lower
//  - lengths from the bones' world positions (they don't change as they turn), directions at rest from their local
//    positions, so any scale works; bones between the shoulder and elbow are taken at rest
//  - the elbow bends down, back and out from the body, from the chest's facing
//  - with IKTargetRotation the hand turns to it too (XR: the body's hand matches the real one)
//  - weight 0: the elbow and hand ease back straight, the shoulder is left to its animation
//  - the shoulder's world position is last frame's (the hierarchy runs after), a moving body lags its hand a frame
#define ik_elbow_return_rate 18.0f

void arm_ik_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const float delta = zox_delta_time < 0.1f ? zox_delta_time : 0.1f;
    const float return_t = 1.0f - expf(-ik_elbow_return_rate * delta);
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
        ik_arm arm;
        if (!ik_arm_find(world, e, &arm)) {
            continue;
        }
        const byte hand_index = arm.length - 1;
        const byte elbow_index = arm.length - 2;
        const entity hand = arm.bones[hand_index];
        const entity elbow = arm.bones[elbow_index];
        LocalRotation3D* elbow_rotation = zox_getm(elbow, LocalRotation3D);
        const float w = weight->value < 0 ? 0 : (weight->value > 1 ? 1 : weight->value);
        if (w <= 0.001f) {
            if (!float4_equals(elbow_rotation->value, quaternion_identity)) {
                elbow_rotation->value = quaternion_normalize2(
                    quaternion_slerp(elbow_rotation->value, quaternion_identity, return_t));
            }
            if (zox_has(e, IKTargetRotation)) {
                LocalRotation3D* hand_rotation = zox_getm(hand, LocalRotation3D);
                if (!float4_equals(hand_rotation->value, quaternion_identity)) {
                    hand_rotation->value = quaternion_normalize2(
                        quaternion_slerp(hand_rotation->value, quaternion_identity, return_t));
                }
            }
            continue;
        }
        const entity parent = zox_get_parent(world, e);
        const float4 parent_rotation = zox_valid(parent) && zox_has(parent, Rotation3D) ?
            zox_getv(parent, Rotation3D) : quaternion_identity;
        const float3 shoulder_point = position->value;
        const float a = float3_distance(shoulder_point, zox_getv(elbow, Position3D));
        const float b = float3_distance(zox_getv(elbow, Position3D), zox_getv(hand, Position3D));
        const float3 upper_rest = ik_arm_rest(world, &arm, 0, elbow_index);
        const float3 lower_rest = ik_arm_rest(world, &arm, elbow_index, hand_index);
        if (a < 0.0001f || b < 0.0001f || float3_length(upper_rest) < 0.0001f || float3_length(lower_rest) < 0.0001f) {
            continue;
        }
        // down, back and out from the body: the right shoulder is on -x (parts/sys/bones.c)
        const float out = zox_getv(e, LocalPosition3D).x < 0 ? -0.5f : 0.5f;
        const float3 pole = float4_rotate_float3(parent_rotation, (float3) { out, -1, -0.6f });
        float3 elbow_point, hand_point;
        ik_two_bone(shoulder_point, a, b, target->value, pole, &elbow_point, &hand_point);
        // the shoulder: its rest upper arm onto the shoulder to elbow, in the parent's space
        const float4 parent_inverse = quaternion_inverse(parent_rotation);
        const float4 shoulder_ik = quaternion_from_to(upper_rest,
            float4_rotate_float3(parent_inverse, float3_sub(elbow_point, shoulder_point)));
        // the elbow's parent in the world: the shoulder turned, then the bones between at their rotations
        float4 elbow_parent = quaternion_rotate(parent_rotation, shoulder_ik);
        for (byte j = 1; j < elbow_index; j++) {
            elbow_parent = quaternion_rotate(elbow_parent, zox_getv(arm.bones[j], LocalRotation3D));
        }
        const float4 elbow_ik = quaternion_from_to(lower_rest,
            float4_rotate_float3(quaternion_inverse(elbow_parent), float3_sub(hand_point, elbow_point)));
        rotation->value = quaternion_normalize2(quaternion_slerp(rotation->value, shoulder_ik, w));
        elbow_rotation->value = quaternion_normalize2(quaternion_slerp(quaternion_identity, elbow_ik, w));
        if (zox_has(e, IKTargetRotation)) {
            // the hand's world turn: the target's, its rest down onto the target's forward; local to the elbow's turn
            const float4 elbow_world = quaternion_rotate(elbow_parent, elbow_ik);
            const float4 down_to_forward = quaternion_from_to(float3_down, float3_backward);
            const float4 hand_world = quaternion_rotate(zox_getv(e, IKTargetRotation), down_to_forward);
            const float4 hand_ik = quaternion_rotate(quaternion_inverse(elbow_world), hand_world);
            zox_getm(hand, LocalRotation3D)->value = quaternion_normalize2(quaternion_slerp(quaternion_identity, hand_ik, w));
        }
    }
    zox_sys_on_end();
} zoxd_system(arm_ik_system);
