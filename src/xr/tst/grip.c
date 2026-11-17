// tst: xr_grip, the body's hands from the controllers' grips, their pointers (the device's pose) from their aims
//  (fun/runtime.c, fun/hands.c); a headset's aim is ahead of the grip and tilted, the hand holds the grip
//  - in a game on the simulator, the right hand's grip held ahead of the player's shoulder, its aim apart from it
//    (0.25m left and up, turned): the body's hand on the grip's target (the same as with the aim on the grip), the
//    right controller's pose on the aim (its world point, on the body)
//  - run: bash bsh/tst.sh xr_grip
int zox_tst_xr_grip_frames = 0;
float3 zox_tst_xr_grip_target_together;     // the hand's target, the aim on the grip
float zox_tst_xr_grip_target_moved = 99;    // how far the target moved with the aim apart
float zox_tst_xr_grip_hand_miss = 99;       // the body's hand from its target
float zox_tst_xr_grip_device_miss = 99;     // the controller's pose from the aim's world point
float zox_tst_xr_grip_device_apart = 0;     // the controller's pose from the body's hand

byte zox_tst_xr_grip_ready(ecs* world) {
    if (zox_tst_xr_play_stage < 3) {
        zox_tst_xr_play_ready(world);
        return 0;
    }
    const entity character = xr_body_character(world);
    if (!zox_valid(character) || !xr_body_head(world, xr_camera)) {
        zox_tst_waiting = "the xr camera on the character's head";
        return 0;
    }
    const int f = ++zox_tst_xr_grip_frames;
    zox_tst_progress(f);
    zox_tst_waiting = "the grip held, its aim apart";
    const float player = xr_player_height / 1.7f;
    const float3 shoulder = float3_scale((float3) { 0.19f, -0.2f, 0.08f }, player);
    const float3 grip = float3_add(shoulder, float3_scale((float3) { 0, 0, -1 }, 0.8f * 0.6f * player));
    xr_sim_script_hands_on = 1;
    xr_sim_script_hands[0] = float3_scale((float3) { -0.3f, -0.6f, -0.2f }, player);
    xr_sim_script_hands[1] = grip;
    // first the aim on the grip, then apart
    const byte apart = f > 40;
    xr_sim_script_aims_on = apart;
    xr_sim_script_aims[0] = xr_sim_script_hands[0];
    xr_sim_script_aims[1] = float3_add(grip, (float3) { -0.25f, 0.25f, 0 });
    xr_sim_script_aim_rotations[1] = quaternion_from_euler((float3) { -0.7f, 0, 0 });
    const entity shoulder_bone = xr_hands_shoulder(world, get_skeleton(world, character), xr_hand_right);
    ik_arm arm;
    if (!zox_valid(shoulder_bone) || !ik_arm_find(world, shoulder_bone, &arm)) {
        return f > 120;
    }
    if (f == 39) {
        zox_tst_xr_grip_target_together = zox_getv(shoulder_bone, IKTarget);
    }
    if (f == 79) {
        const float3 target = zox_getv(shoulder_bone, IKTarget);
        const float3 hand = zox_getv(arm.bones[arm.length - 1], Position3D);
        const entity device = xr_controllers[xr_hand_right];
        zox_tst_xr_grip_target_moved = float3_distance(target, zox_tst_xr_grip_target_together);
        zox_tst_xr_grip_hand_miss = float3_distance(hand, target);
        xr_body_frame frame;
        if (zox_valid(device) && xr_body_frame_get(world, xr_camera, xr_head_now_rotation, xr_head_now_position, &frame)) {
            const float3 pose = zox_getv(device, Position3D);
            zox_tst_xr_grip_device_miss = float3_distance(pose, xr_body_frame_point(frame, xr_sim_script_aims[1]));
            zox_tst_xr_grip_device_apart = float3_distance(pose, hand);
        }
        xr_sim_script_hands_on = 0;
        xr_sim_script_aims_on = 0;
        return 1;
    }
    return 0;
}

void zox_tst_xr_grip_check(ecs* world) {
    (void) world;
    zox_log("[tst] xr_grip: the hand's target moved [%.4f] with the aim apart, the hand off it [%.4f], the controller off the aim [%.4f], from the hand [%.3f]",
        zox_tst_xr_grip_target_moved, zox_tst_xr_grip_hand_miss, zox_tst_xr_grip_device_miss, zox_tst_xr_grip_device_apart);
    zox_tst_check(zox_tst_xr_grip_target_moved < 0.01f, "the body's hand followed the aim [%.4f], not the grip", zox_tst_xr_grip_target_moved);
    zox_tst_check(zox_tst_xr_grip_hand_miss < 0.01f, "the body's hand is off its grip's target [%.4f]", zox_tst_xr_grip_hand_miss);
    zox_tst_check(zox_tst_xr_grip_device_miss < 0.01f, "the controller's pose isn't its aim [%.4f]", zox_tst_xr_grip_device_miss);
    zox_tst_check(zox_tst_xr_grip_device_apart > 0.1f, "the controller's pose is on the hand [%.3f], the aim wasn't apart", zox_tst_xr_grip_device_apart);
}

void zox_tsts_xr_grip(ecs* world) {
    zox_tst_add("xr_grip", zox_tst_xr_grip_ready, zox_tst_xr_grip_check, 120);
    zox_tst_set_setup("xr_grip", zox_tst_xr_sim_setup, 1);
}
