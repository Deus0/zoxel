// tst: free_roam_pose, the body keeps its first person pose while the camera roams (F1), to be seen from outside
//  (controllers3/sys/freeroam.c set_camera_free, bones LookHeld, first_person_look)
//  - the player's guard raised, the camera pitched up 70 (as a mouse turns it): the hand's place from the body
//  - the camera set free (free roam): a second on, the hand's where it was (it fell back to the level guard), the
//    camera off the head
//  - the camera back on the character: its look the camera's again (LookHeld gone)
//  - run: bash bsh/tst.sh free_roam_pose
byte zox_tst_frp_stage = 0;
double zox_tst_frp_time = 0;
float3 zox_tst_frp_hand[2];     // the hand from the character, in its space: first person, roaming
byte zox_tst_frp_held = 0, zox_tst_frp_released = 0;

static inline float3 zox_tst_frp_hand_local(ecs* world, entity character, entity hand) {
    return float4_rotate_float3(quaternion_inverse(zox_getv(character, Rotation3D)),
        float3_sub(zox_getv(hand, Position3D), zox_getv(character, Position3D)));
}

byte zox_tst_free_roam_pose_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = zox_get_link(world, character, SkeletonLink);
    const entity shoulder = zox_valid(skeleton) ? zox_get_link(world, skeleton, ShoulderBoneLink) : 0;
    const entity camera = zox_get_link(world, character, CameraLink);
    ik_arm arm;
    if (!zox_valid(camera) || !zox_valid(shoulder) || !ik_arm_find(world, shoulder, &arm)) {
        zox_tst_waiting = "the player's arm and camera";
        return 0;
    }
    const entity hand = arm.bones[arm.length - 1];
    zox_getm(skeleton, RaiseShoulder)->value = 1;
    if (!zox_tst_frp_time) {
        zox_tst_frp_time = zox_current_time;
    }
    const double t = zox_current_time - zox_tst_frp_time;
    zox_tst_progress(zox_tst_frp_stage * 100 + (int) (t * 10));
    if (zox_tst_frp_stage == 0) {
        zox_tst_waiting = "the camera pitched up";
        // up 70, in mouse sized steps (camera_pitch_add)
        const float4 local = zox_getv(camera, LocalRotation3D);
        const float3 ahead = float4_rotate_float3(local, (float3) { 0, 0, -1 });
        const float now = asinf(float_max(-1, float_min(1, ahead.y)));
        const float pitch = 70 * degrees_to_radians;
        if (pitch - now > 0.001f) {
            const float3 probed = float4_rotate_float3(camera_pitch_add(local, 0.01f, 89 * degrees_to_radians), (float3) { 0, 0, -1 });
            const float up_sign = probed.y > ahead.y ? 1 : -1;
            zox_setv(camera, LocalRotation3D, camera_pitch_add(local, up_sign * float_min(2 * degrees_to_radians, pitch - now), 89 * degrees_to_radians));
            return 0;
        }
        if (t < 1.5) {
            return 0;
        }
        zox_tst_frp_hand[0] = zox_tst_frp_hand_local(world, character, hand);
        set_camera_free(world, camera);
        zox_setv(camera, CameraState, zox_camera_state_free);
        zox_tst_frp_stage = 1;
        zox_tst_frp_time = zox_current_time;
        return 0;
    }
    if (zox_tst_frp_stage == 1) {
        zox_tst_waiting = "the body posed a second, the camera roaming";
        if (t < 1) {
            return 0;
        }
        zox_tst_frp_hand[1] = zox_tst_frp_hand_local(world, character, hand);
        zox_tst_frp_held = zox_has(skeleton, LookHeld);
        zox_remove(camera, Roaming);
        attach_camera_to_character(world, camera, zox_tst_player_character(world));
        zox_tst_frp_stage = 2;
        zox_tst_frp_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the camera back on the character";
    if (t < 0.5) {
        return 0;
    }
    zox_tst_frp_released = !zox_has(skeleton, LookHeld);
    zox_tst_frp_stage = 3;
    return 1;
}

void zox_tst_free_roam_pose_check(ecs* world) {
    (void) world;
    const float3 a = zox_tst_frp_hand[0], b = zox_tst_frp_hand[1];
    const float moved = float3_distance(a, b);
    zox_log("[tst] [metric] free_roam_pose: the hand [%.3f %.3f %.3f] first person, [%.3f %.3f %.3f] roaming, moved [%.3f]",
        a.x, a.y, a.z, b.x, b.y, b.z, moved);
    zox_tst_check(zox_tst_frp_stage == 3, "stopped at stage [%i]", zox_tst_frp_stage);
    zox_tst_check(zox_tst_frp_held, "the camera roaming, the skeleton didn't hold its look (LookHeld)");
    zox_tst_check(moved < 0.05f, "the camera roaming, the hand moved [%.3f] (its pose reset)", moved);
    zox_tst_check(zox_tst_frp_released, "the camera back, the skeleton still holds a look");
}
