// tst: xr_look_down, looking down (the head pitched, the neck still) the eyes stay out of the body's chest
//  (fun/body.c): in a game on the simulator, the head pitched down from level to 1.3 radians over 3 seconds; each
//  frame the eyes (on the body) against the chest, in the chest's frame (it pitches forward looking down): below
//  the neck joint along the chest's up and nearer ahead than its front (its half depth) is inside it
//  - the upper body pitches forward a little, the hips back (fun/body.c xr_look_bend), the eyes stay the player's
//  - then crouched 0.3m, still looking down
//  - and the eyes out of every part of the body, the head's too (tst/eyes.c): the head turned by the chest's lean
//    on top of the headset's and the body's neck sank under the eyes, they went through the head to its back
//  - run: bash bsh/tst.sh xr_look_down
int zox_tst_xr_look_frames = 0;
float zox_tst_xr_look_worst = -99;      // the most the eyes were inside the chest, metres (under 0: out of it)
float zox_tst_xr_look_worst_pitch = 0;
int zox_tst_xr_look_inside = 0;         // frames inside
float zox_tst_xr_look_hips_back = 0;    // looking down most, the hips' shift back (the body's space)
float zox_tst_xr_look_pitch = 0;        // and the spine's lean, radians

byte zox_tst_xr_look_down_ready(ecs* world) {
    if (zox_tst_xr_play_stage < 3) {
        zox_tst_xr_play_ready(world);
        return 0;
    }
    const entity character = xr_body_character(world);
    if (!zox_valid(character) || !xr_body_head(world, xr_camera)) {
        zox_tst_waiting = "the xr camera on the character's head";
        return 0;
    }
    const int f = ++zox_tst_xr_look_frames;
    zox_tst_progress(f);
    zox_tst_waiting = "looking down";
    const float pitch = f < 30 ? 0 : float_min(1.3f, (f - 30) / 180.0f * 1.3f);
    xr_sim_head_euler = (float3) { -pitch, 0, 0 };
    if (f < 40) {
        return 0;
    }
    const entity skeleton = get_skeleton(world, character);
    spine_bones spine;
    xr_body_frame frame;
    if (!zox_valid(skeleton) || !spine_bones_find(world, skeleton, &spine) ||
        !xr_body_frame_get(world, xr_camera, xr_head_now_rotation, xr_head_now_position, &frame)) {
        return f > 400;
    }
    zox_tst_xr_eyes_measure(world, f > 260 ? "crouched looking down" : "looking down");
    if (f == 250) {
        zox_tst_xr_eyes_screenshot("look_down");
    }
    const float3 eye = xr_body_frame_point(frame, xr_head_now_position);
    const float3 neck = zox_getv(spine.head, Position3D);
    const float4 chest_rotation = zox_getv(spine.chest, Rotation3D);
    const float3 local = float4_rotate_float3(quaternion_inverse(chest_rotation), float3_sub(eye, neck));
    const float scale = zox_has(character, Scale1) ? zox_getv(character, Scale1) : 1;
    const float chest_depth = zox_has(spine.chest, BoneSize) ? zox_getv(spine.chest, BoneSize).z * scale : 0.1f;
    const float drop = -local.y;
    const float forward = local.z;
    // inside: below the neck, nearer ahead than the chest's front
    const float inside = drop > 0 ? chest_depth - forward : -99;
    if (inside > zox_tst_xr_look_worst) {
        zox_tst_xr_look_worst = inside;
        zox_tst_xr_look_worst_pitch = pitch;
    }
    zox_tst_xr_look_inside += inside > 0;
    if (f % 30 == 0) {
        zox_log("[tst] xr_look_down: pitch [%.2f] the eyes below the neck [%.3f], ahead of it [%.3f], the chest's half depth [%.3f]",
            pitch, drop, forward, chest_depth);
    }
    if (f == 255 && zox_has(skeleton, HipsShift) && zox_has(skeleton, SpineLean)) {
        zox_tst_xr_look_hips_back = -zox_getv(skeleton, HipsShift).z;
        zox_tst_xr_look_pitch = 2 * acosf(float_min(1, fabsf(zox_getv(skeleton, SpineLean).w)));
    }
    // then crouched 0.3m, still looking down
    xr_sim_script_head_position = (float3) { 0, f > 260 ? -0.3f * float_min(1, (f - 260) / 40.0f) : 0, 0 };
    if (f == 340) {
        zox_tst_xr_eyes_screenshot("crouch_look_down");
    }
    if (f > 350) {
        xr_sim_head_euler = float3_zero;
        xr_sim_script_head_position = float3_zero;
    }
    return f > 350;
}

void zox_tst_xr_look_down_check(ecs* world) {
    (void) world;
    zox_log("[tst] xr_look_down: inside the chest [%i] frames, the most [%.3f] at pitch [%.2f]",
        zox_tst_xr_look_inside, zox_tst_xr_look_worst, zox_tst_xr_look_worst_pitch);
    zox_log("[tst] xr_look_down: looking down most, the hips back [%.3f], the spine pitched [%.2f]", zox_tst_xr_look_hips_back, zox_tst_xr_look_pitch);
    zox_tst_check(zox_tst_xr_look_hips_back > 0.02f, "looking down the hips didn't go back [%.3f]", zox_tst_xr_look_hips_back);
    zox_tst_check(zox_tst_xr_look_pitch > 0.2f, "looking down the upper body didn't pitch forward [%.2f]", zox_tst_xr_look_pitch);
    zox_tst_xr_eyes_check("xr_look_down");
    zox_tst_check(zox_tst_xr_look_inside == 0, "looking down the eyes went into the chest [%i] frames, [%.3f] at pitch [%.2f]",
        zox_tst_xr_look_inside, zox_tst_xr_look_worst, zox_tst_xr_look_worst_pitch);
}

void zox_tsts_xr_look_down(ecs* world) {
    zox_tst_add("xr_look_down", zox_tst_xr_look_down_ready, zox_tst_xr_look_down_check, 120);
    zox_tst_set_setup("xr_look_down", zox_tst_xr_sim_setup, 1);
}
