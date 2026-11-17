// tst: xr_lean, the XR body following the player's body past standing and looking about (fun/body.c), on the
//  simulator: xr_body's script (tst/body.c) on from its end, its own checks (xr_body alone ran past the test's time)
//  - crouching 0.3m: the eyes go 0.3m lower (scaled), the hips down (and a little back), the knees bent, the body stays
//  - turning round in the room (the head 180 degrees): the body turns after it, to within xr_body_follow_start, the
//    room (and the view) still in the world (the head bone twisted round alone)
//  - leaning 0.8m at a headset's frame rate (dt 1/72): the eyes all the way, the body moving after past the spine's
//    reach
//  - bending down (the neck 0.3m ahead, 0.5m lower): the hips back, the spine pitched down, the body still, the eyes
//    on the player's, the view on the body's neck (bending read as leaning: the body walked, the eyes in the chest)
//  - recentred (the system button): the room's turn goes to the body, the view still, the room straight on it
//  - run: bash bsh/tst.sh xr_lean
byte zox_tst_xr_lean_started = 0;

byte zox_tst_xr_lean_ready(ecs* world) {
    if (zox_tst_xr_play_stage < 3) {
        zox_tst_xr_play_ready(world);
        return 0;
    }
    if (!zox_tst_xr_lean_started && zox_valid(xr_body_character(world)) && xr_body_head(world, xr_camera)) {
        zox_tst_xr_lean_started = 1;
        zox_tst_xr_body_frames = 445;   // xr_body's script from where it ends
        zox_tst_xr_body_end = 700;
    }
    return zox_tst_xr_body_ready(world);
}

void zox_tst_xr_lean_check(ecs* world) {
    const float scale = xr_body_scale(world, xr_body_character(world));
        zox_tst_check(fabsf(zox_tst_xr_body_crouch - 0.3f * scale) < 0.02f, "crouching 0.3m lowered the eyes [%f]", zox_tst_xr_body_crouch);
        zox_log("[tst] xr_lean: crouched, the hips dropped [%.3f], a knee w [%.3f]", zox_tst_xr_body_crouch_drop, zox_tst_xr_body_crouch_knee);
        zox_tst_check(zox_tst_xr_body_crouch_drop > 0.05f, "crouching didn't drop the hips [%f]", zox_tst_xr_body_crouch_drop);
        zox_tst_check(zox_tst_xr_body_crouch_knee < 0.99f, "crouching didn't bend the knees [%f]", zox_tst_xr_body_crouch_knee);
        zox_log("[tst] xr_lean: turned round, the body turned [%.3f] (want [%.3f]), the room moved [%.4f]",
            zox_tst_xr_body_turned, (float) M_PI - xr_body_follow_start, zox_tst_xr_body_turn_room_moved);
        zox_tst_check(fabsf(zox_tst_xr_body_turned - ((float) M_PI - xr_body_follow_start)) < 0.1f, "turning round, the body turned [%f]", zox_tst_xr_body_turned);
        zox_tst_check(zox_tst_xr_body_turn_room_moved < 0.01f, "turning round, the room swung [%f]", zox_tst_xr_body_turn_room_moved);
        zox_log("[tst] xr_lean: leaning 0.8m at 72 fps the body moved [%.3f] (of [%.3f])", zox_tst_xr_body_far_moved, 0.8f * scale);
        zox_tst_check(zox_tst_xr_body_far_moved > 0.3f * scale && zox_tst_xr_body_far_moved < 0.8f * scale, "leaning 0.8m at 72 fps moved the body [%f]", zox_tst_xr_body_far_moved);
        zox_log("[tst] xr_lean: bending down, the hips back [%.3f], the spine pitched [%.3f], the body moved [%.3f], the eyes off the player's [%.4f], the view off the body's neck [%.3f]",
            zox_tst_xr_body_bend_back, zox_tst_xr_body_bend_pitch, zox_tst_xr_body_bend_moved, zox_tst_xr_body_bend_eyes, zox_tst_xr_body_bend_view_off);
        zox_tst_check(zox_tst_xr_body_bend_back < -0.01f, "bending down the hips didn't go back [%f]", zox_tst_xr_body_bend_back);
        zox_tst_check(zox_tst_xr_body_bend_pitch > 0.8f, "bending down the spine didn't pitch [%f]", zox_tst_xr_body_bend_pitch);
        zox_tst_check(zox_tst_xr_body_bend_moved >= 0 && zox_tst_xr_body_bend_moved < 0.03f, "bending down the body walked [%f]", zox_tst_xr_body_bend_moved);
        zox_tst_check(zox_tst_xr_body_bend_eyes >= 0 && zox_tst_xr_body_bend_eyes < 0.01f, "bending down the eyes left the player's [%f]", zox_tst_xr_body_bend_eyes);
        zox_tst_check(zox_tst_xr_body_bend_view_off >= 0 && zox_tst_xr_body_bend_view_off < 0.03f, "bending down the view left the body's neck [%f] (into the body)", zox_tst_xr_body_bend_view_off);
        zox_log("[tst] xr_lean: recentred, the view turned [%.4f], the room on the body [%.4f]", zox_tst_xr_body_recentre_room_moved, zox_tst_xr_body_recentre_turn);
        zox_tst_check(zox_tst_xr_body_recentre_room_moved >= 0 && zox_tst_xr_body_recentre_room_moved < 0.01f, "recentring turned the view [%f]", zox_tst_xr_body_recentre_room_moved);
        zox_tst_check(zox_tst_xr_body_recentre_turn >= 0 && zox_tst_xr_body_recentre_turn < 0.01f, "recentred, the room's still turned on the body [%f]", zox_tst_xr_body_recentre_turn);
        zox_tst_check(zox_tst_xr_body_crouch_drift >= 0 && zox_tst_xr_body_crouch_drift < 0.01f, "crouching moved the body [%f]", zox_tst_xr_body_crouch_drift);
}

void zox_tsts_xr_lean(ecs* world) {
    zox_tst_add("xr_lean", zox_tst_xr_lean_ready, zox_tst_xr_lean_check, 150);
    zox_tst_set_setup("xr_lean", zox_tst_xr_sim_setup, 1);
}
