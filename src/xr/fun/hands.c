// XR hands: the player's arms reach for their controllers, with ik (bones/ik), so the body's hands are the
//  player's own
//  - each controller's tracked pose (this frame's, zoxp_inputs_extract), on the body where it is after physics
//    (zoxp_trails, xr_body_frame_get), is its shoulder's IKTarget, full weight, written in place so the solve
//    (zoxp_layouts) reads it the same frame: the hand stays on the controller as the body walks
//  - the hand turns as the controller does (IKTargetRotation), the body's hand matches the real one
//  - the shoulders are held (IKHeld): a punch leaves them be, the real hand punches
//  - the right shoulder is on -x (parts/sys/bones.c)
//  - waits on a player body (doc/plans/xr.md step 5): without a character's skeleton it does nothing
static entity xr_hands_shoulder(ecs* world, entity skeleton, byte hand) {
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    if (!bones) {
        return 0;
    }
    for (int i = 0; i < bones->length; i++) {
        const entity bone = bones->value[i];
        if (!zox_valid(bone) || !zox_has(bone, ShoulderBone) || !zox_has(bone, IKWeight)) {
            continue;
        }
        const byte right = zox_getv(bone, LocalPosition3D).x < 0;
        if (right == (hand == xr_hand_right)) {
            return bone;
        }
    }
    return 0;
}

// the player's shoulder from their neck (in the body's space, as a 1.7m player's: the right on -x), their arm's
//  length shoulder to grip; scaled by their height (XR Height)
#define xr_player_shoulder ((float3) { 0.19f, -0.10f, 0 })
#define xr_player_arm 0.6f

// the hand's target, from the shoulders: the body's shoulder (leaned as it is now), the player's hand off their own
//  shoulder (from their neck, the body's facing and lean), scaled by the body's arm over theirs; a full arm is a full
//  arm every way (from the eyes, the body's shoulders lower, further back and wider than the player's scaled: its
//  arm overreached up and ahead, the hand short of the controller, half bent down and out, tst xr_reach)
static float3 xr_hands_target(ecs* world, entity skeleton, entity shoulder, byte hand, xr_body_frame frame) {
    ik_arm arm;
    if (!ik_arm_find(world, shoulder, &arm)) {
        return xr_body_frame_point(frame, xr_hand_tracked_position[hand]);
    }
    const float player = xr_player_height / 1.7f;
    const float4 lean = zox_has(skeleton, SpineLean) ? zox_getv(skeleton, SpineLean) : quaternion_identity;
    const float3 shift = zox_has(skeleton, HipsShift) ? zox_getv(skeleton, HipsShift) : float3_zero;
    // the player's shoulder in the room
    const float3 neck = float3_sub(xr_head_now_position, float4_rotate_float3(xr_head_now_rotation, xr_neck_to_eyes));
    float3 side = float3_scale(xr_player_shoulder, player);
    side.x *= hand == xr_hand_right ? -1 : 1;
    const float3 player_shoulder = float3_add(neck,
        float4_rotate_float3(quaternion_inverse(xr_body_room_to_body()), float4_rotate_float3(lean, side)));
    // the body's shoulder in the world, now, at rest (the arm's ik moves it after a target past its reach: from where
    //  it's moved the target went with it each frame, short again)
    const float3 shoulder_rest = zox_has(shoulder, RestPosition3D) ? zox_getv(shoulder, RestPosition3D) : zox_getv(shoulder, LocalPosition3D);
    const skeleton_now now = skeleton_now_get(world, skeleton);
    float3 body_shoulder = skeleton_now_point(&now, zox_getv(shoulder, Position3D));
    spine_bones spine;
    if (zox_has(skeleton, SpineLean) && spine_bones_find(world, skeleton, &spine) && zox_has(spine.chest, RestPosition3D) &&
        zox_get_parent(world, shoulder) == spine.chest) {
        body_shoulder = float3_add(now.now_position, float4_rotate_float3(now.now_rotation,
            spine_point_local(world, &spine, shoulder_rest, lean, shift)));
    }
    const float3 elbow = zox_getv(arm.bones[arm.length - 2], Position3D);
    const float reach = float3_distance(zox_getv(shoulder, Position3D), elbow) +
        float3_distance(elbow, zox_getv(arm.bones[arm.length - 1], Position3D));
    const float3 out = float3_scale(float3_sub(xr_hand_tracked_position[hand], player_shoulder), reach / (xr_player_arm * player));
    return float3_add(body_shoulder, float4_rotate_float3(frame.rotation, out));
}

// multi threaded, the xr camera its one entity (one worker runs it): it writes only the shoulders' ik targets and
//  weights, read after the barrier by arm_ik_system (zoxp_layouts); it reads the spine's lean and shift (written in
//  zoxp_update) and the bones' world transforms (the hierarchy's, later): none written in zoxp_trails
void xr_hands_ik_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    if (!it->count) {
        return;
    }
    const entity player = zox_players[0];
    if (!zox_xr_active || !zox_valid(player)) {
        return;
    }
    const entity skeleton = get_skeleton(world, zox_get_link(world, player, CharacterLink));
    xr_body_frame frame;
    if (!zox_valid(skeleton) || !xr_body_frame_get(world, xr_camera, xr_head_now_rotation, xr_head_now_position, &frame)) {
        return;
    }
    for (byte hand = 0; hand < 2; hand++) {
        const entity shoulder = xr_hands_shoulder(world, skeleton, hand);
        if (!xr_hand_tracked[hand] || !zox_valid(shoulder)) {
            continue;
        }
        const float4 rotation = quaternion_rotate(frame.rotation, xr_hand_tracked_rotation[hand]);
        if (!zox_has(shoulder, IKHeld)) {
            zox_add(shoulder, IKHeld);
            zox_setv(shoulder, IKTargetRotation, rotation);
        } else {
            zox_getm(shoulder, IKTargetRotation)->value = rotation;
        }
        zox_getm(shoulder, IKTarget)->value = xr_hands_target(world, skeleton, shoulder, hand, frame);
        zox_getm(shoulder, IKWeight)->value = 1;
    }
    zox_sys_on_end();
} zoxd_system(xr_hands_ik_system);

void zox_systems_xr_hands(ecs* world) {
    // after physics moved the body, before the arms' ik (zoxp_layouts)
    zox_system(xr_hands_ik_system, zoxp_trails, [none] cameras.XrCamera);
    zox_set_enabled_id(ecs_id(xr_hands_ik_system), 0);
}

// the xr systems run only with a headset (or the simulator): zox_xr_active is set by now (arguments, tests' setup)
void xr_systems_boot(ecs* world, entity app) {
    (void) app;
    if (!zox_xr_active) {
        return;
    }
    zox_set_enabled_id(ecs_id(xr_room_walk_system), 1);
    zox_set_enabled_id(ecs_id(xr_head_bone_system), 1);
    zox_set_enabled_id(ecs_id(xr_hands_ik_system), 1);
}
