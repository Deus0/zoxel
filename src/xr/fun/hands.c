// XR hands: the player's arms reach for their controllers, with ik (bones/ik), so the body's hands are the
//  player's own
//  - each controller's pose (written this frame, zoxp_inputs_extract) is its shoulder's IKTarget, full weight,
//    written in place so the solve (zoxp_post_update) reads it the same frame
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
    if (!zox_valid(skeleton)) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(XrHand);
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(XrHand, hand);
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        const entity shoulder = xr_hands_shoulder(world, skeleton, hand->value);
        if (!zox_valid(shoulder)) {
            continue;
        }
        if (!zox_has(shoulder, IKHeld)) {
            zox_add(shoulder, IKHeld);
            zox_setv(shoulder, IKTargetRotation, rotation->value);
        } else {
            zox_getm(shoulder, IKTargetRotation)->value = rotation->value;
        }
        zox_getm(shoulder, IKTarget)->value = position->value;
        zox_getm(shoulder, IKWeight)->value = 1;
    }
    zox_sys_on_end();
} zoxd_system(xr_hands_ik_system);

void zox_systems_xr_hands(ecs* world) {
    zox_system_1(
        xr_hands_ik_system,
        zoxp_update,
        [in] xr.XrHand,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [none] xr.XrController
    );
}
