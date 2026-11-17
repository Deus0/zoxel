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

void xr_hands_ik_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
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
        zox_getm(shoulder, IKTarget)->value = xr_body_frame_point(frame, xr_hand_tracked_position[hand]);
        zox_getm(shoulder, IKWeight)->value = 1;
    }
    zox_sys_on_end();
} zoxd_system(xr_hands_ik_system);

void zox_systems_xr_hands(ecs* world) {
    // after physics moved the body, before the arms' ik (zoxp_layouts)
    zox_system_1(xr_hands_ik_system, zoxp_trails, 0);
}
