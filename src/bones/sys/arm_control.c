// Tells the limbs to swing or not, and keeps the skeleton's swing timing, shared by all its limbs
//  - the skeleton's SwingPhase and SwingBlend advance here once, and its limbs are posed from them here too
//    (arm_swing.c), so both arms and legs stay in step (each limb kept its own phase, a raised arm fell out of step)
//  - phase accumulates, so changing speed never jumps the angle, SwingBlend eases idle <-> walking
//  - walking, a step a half cycle, as many as the body's speed over the stride (both legs swing their SwingAngle each
//    way: 2 L sin A) so the feet don't skate; a fixed pace slid them, slow and fast alike
//  - the body bobs over its stance foot: the root bone down by what the longer leg lost of its height (its thigh and
//    knee this frame), so that foot stays on the ground as the legs spread, highest as they pass (the head and a
//    first person camera with it, in step)
//  - crouching (CrouchState, eased into CrouchBlend) the knees bend (arm_swing.c) and the body goes down by what the
//    legs lost, the feet on the ground
// TODO: Only signal the bones when WalkStateDirty
#define swing_idle_speed 0.1f   // half cycles per second
#define swing_walk_speed 1.8f   // without legs to stride with
#define swing_steps_min 1.2f    // steps a second walking, the most and least
#define swing_steps_max 4.0f
#define swing_blend_rate 5.0f   // how fast idle <-> walk eases
#define crouch_blend_rate 10.0f // how fast standing <-> crouched eases
#define swing_max_delta 0.1f

// a leg's thigh and shin heights at rest (the hip to the knee, the knee to the ankle: their pivots), 0 without both
static inline byte walk_leg_heights(ecs* world, entity thigh, float* upper, float* lower) {
    const entity shin = zox_get_child_by_id(world, thigh, zox_id(Bone));
    const entity foot = zox_valid(shin) ? zox_get_child_by_id(world, shin, zox_id(Bone)) : 0;
    if (!zox_valid(foot)) {
        return 0;
    }
    *upper = zox_getv(thigh, BonePosition).y - zox_getv(shin, BonePosition).y;
    *lower = zox_getv(shin, BonePosition).y - zox_getv(foot, BonePosition).y;
    return 1;
}

void arm_control_system(iter* it) {
    zox_sys_on_begin();
    const float delta = zox_delta_time < swing_max_delta ? zox_delta_time : swing_max_delta;
    const float blend_t = 1.0f - expf(-swing_blend_rate * delta);
    const float follow_t = 1.0f - expf(-walk_follow_rate * delta);
    const float crouch_t = 1.0f - expf(-crouch_blend_rate * delta);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WalkState);
    zox_sys_out(SwingPhase);
    zox_sys_out(SwingBlend);
    zox_sys_in(CrouchState);
    zox_sys_out(CrouchBlend);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(WalkState, state);
        zox_sys_o(SwingPhase, phase);
        zox_sys_o(SwingBlend, blend);
        zox_sys_i(CrouchState, crouch_state);
        zox_sys_o(CrouchBlend, crouch);
        crouch->value += ((crouch_state->value ? 1.0f : 0.0f) - crouch->value) * crouch_t;
        if (crouch->value < 0.001f) {
            crouch->value = 0;
        }
        entity bones[zox_children_capacity];
        uint length = zox_get_children_by_id_recursive(world, e, bones, zox_children_capacity, zox_id(SwingState), 0);
        float leg = 0;              // hip to ankle at rest
        float stride_angle = 0;     // radians each way
        for (uint j = 0; j < length; j++) {
            SwingState* swing = zox_getm(bones[j], SwingState);
            if (swing->value) {
                // NOTE: 1 is idle and 2 is moving
                swing->value = state->value + 1;
            }
            float upper, lower;
            if (!leg && zox_has(bones[j], ThighBone) && walk_leg_heights(world, bones[j], &upper, &lower)) {
                leg = upper + lower;
                stride_angle = zox_getv(bones[j], SwingAngle) * degrees_to_radians;
            }
        }
        // the skeleton's timing
        blend->value += ((state->value ? 1.0f : 0.0f) - blend->value) * blend_t;
        float steps = swing_walk_speed;
        const entity owner = zox_get_parent(world, e);
        const float stride = 2 * leg * sinf(stride_angle) * (1 - crouch->value * 0.3f);    // crouched, shorter
        if (stride > 0.01f && zox_valid(owner) && zox_has(owner, Velocity3D)) {
            const float3 velocity = zox_getv(owner, Velocity3D);
            steps = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z) / stride;
            steps = steps < swing_steps_min ? swing_steps_min : (steps > swing_steps_max ? swing_steps_max : steps);
        }
        phase->value += M_PI * (swing_idle_speed + (steps - swing_idle_speed) * blend->value) * delta;
        if (phase->value > 2 * M_PI) {
            phase->value -= 2 * M_PI;
        }
        // its limbs on the swing (arm_swing.c), raised ones left be (SwingState 0: arm_raise.c, the ik)
        for (uint j = 0; j < length; j++) {
            if (zox_getv(bones[j], SwingState)) {
                walk_limb(world, bones[j], phase->value, blend->value, crouch->value, zox_has(bones[j], ThighBone), follow_t);
            }
        }
        // the body over its stance foot: down by what the longer leg (its foot the lower) lost of its height, its
        //  thigh and knee as walk_limb put them this frame (arm_swing.c)
        const entity root = zox_get_child_by_id(world, e, zox_id(Bone));
        // (not a leaning body's: XR's, sys/spine.c keeps the root's rest place, RestPosition3D, and places it)
        if (leg > 0 && zox_valid(root) && !zox_has(root, RestPosition3D)) {
            float reach = 0;
            for (uint j = 0; j < length; j++) {
                float upper, lower, thigh, knee;
                if (zox_has(bones[j], ThighBone) && zox_getv(bones[j], SwingState) && walk_leg_heights(world, bones[j], &upper, &lower)) {
                    walk_leg_angles(zox_getv(bones[j], SwingAngle), zox_getv(bones[j], LocalPosition3D).x, phase->value,
                        blend->value, crouch->value, &thigh, &knee);
                    reach = fmaxf(reach, upper * cosf(thigh * degrees_to_radians) + lower * cosf((thigh + knee) * degrees_to_radians));
                }
            }
            //  written in place, not deferred (zox_setv): a frame behind the legs it was most of a step at 10 fps
            float3 local = zox_getv(root, BonePosition);
            local.y -= leg - (reach > 0 ? reach : leg);
            zox_getm(root, LocalPosition3D)->value = local;
        }
    }
    zox_sys_on_end();
} zoxd_system(arm_control_system);
