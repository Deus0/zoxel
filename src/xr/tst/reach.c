// tst: xr_reach, the body's arm reaches as the player's does, every way (xr/fun/hands.c, bones/ik)
//  - in a game on the simulator, the head level at the room's origin; the right hand held at 0.8 of a real arm's
//    length (0.6m of a 1.7m player) from where the player's right shoulder is, ahead, up, down and out to the side
//  - each way, the body's arm is 0.8 out too (its hand from its shoulder, of its reach), its hand on its target:
//    from the eyes it overreached up and ahead (the hand short of the controller) and was half bent down and out
//  - up and out past a real arm (1.15 of it): the arm straight, its hand still on its target (the shoulder moving
//    after it, bones/ik arm_ik.c: the arm alone fell short)
//  - run: bash bsh/tst.sh xr_reach
#define xr_tst_reach_ways 6
const char* zox_tst_xr_reach_names[xr_tst_reach_ways] = { "ahead", "up", "down", "side", "up_far", "side_far" };
const float3 zox_tst_xr_reach_ways[xr_tst_reach_ways] = { { 0, 0, -1 }, { 0, 1, 0 }, { 0, -1, 0 }, { 1, 0, 0 }, { 0, 1, 0 }, { 1, 0, 0 } };
// of a real arm: the far ones past it, as a person reaches up and out (their shoulder shrugs, pushes out)
const float zox_tst_xr_reach_held[xr_tst_reach_ways] = { 0.8f, 0.8f, 0.8f, 0.8f, 1.15f, 1.15f };
float zox_tst_xr_reach_out[xr_tst_reach_ways];
float zox_tst_xr_reach_miss[xr_tst_reach_ways];
int zox_tst_xr_reach_frames = 0;

byte zox_tst_xr_reach_ready(ecs* world) {
    if (zox_tst_xr_play_stage < 3) {
        zox_tst_xr_play_ready(world);
        return 0;
    }
    const entity character = xr_body_character(world);
    if (!zox_valid(character) || !xr_body_head(world, xr_camera)) {
        zox_tst_waiting = "the xr camera on the character's head";
        return 0;
    }
    const int f = ++zox_tst_xr_reach_frames;
    zox_tst_progress(f);
    zox_tst_waiting = "reaching each way";
    const int way = f / 40;
    if (way >= xr_tst_reach_ways) {
        xr_sim_script_hands_on = 0;
        return 1;
    }
    // the player's right shoulder from their eyes (the room's -z ahead), their arm's length
    const float player = xr_player_height / 1.7f;
    const float3 shoulder = float3_scale((float3) { 0.19f, -0.2f, 0.08f }, player);
    xr_sim_script_hands_on = 1;
    xr_sim_script_hands[0] = float3_scale((float3) { -0.3f, -0.6f, -0.2f }, player);
    xr_sim_script_hands[1] = float3_add(shoulder, float3_scale(zox_tst_xr_reach_ways[way], zox_tst_xr_reach_held[way] * 0.6f * player));
    if (f % 40 == 39) {
        const entity shoulder_bone = xr_hands_shoulder(world, get_skeleton(world, character), xr_hand_right);
        ik_arm arm;
        if (zox_valid(shoulder_bone) && ik_arm_find(world, shoulder_bone, &arm)) {
            const float3 s = zox_getv(shoulder_bone, Position3D);
            const float3 el = zox_getv(arm.bones[arm.length - 2], Position3D);
            const float3 h = zox_getv(arm.bones[arm.length - 1], Position3D);
            zox_tst_xr_reach_out[way] = float3_distance(s, h) / (float3_distance(s, el) + float3_distance(el, h));
            zox_tst_xr_reach_miss[way] = float3_distance(h, zox_getv(shoulder_bone, IKTarget));
        }
    }
    return 0;
}

void zox_tst_xr_reach_check(ecs* world) {
    (void) world;
    for (int i = 0; i < xr_tst_reach_ways; i++) {
        const float held = float_min(1, zox_tst_xr_reach_held[i]);
        zox_log("[tst] xr_reach: [%s] the arm out [%.3f] of its reach (want [%.2f]), the hand off its target [%.4f]",
            zox_tst_xr_reach_names[i], zox_tst_xr_reach_out[i], held, zox_tst_xr_reach_miss[i]);
        zox_tst_check(fabsf(zox_tst_xr_reach_out[i] - held) < 0.03f, "reaching [%s] the arm's [%.3f] out", zox_tst_xr_reach_names[i], zox_tst_xr_reach_out[i]);
        zox_tst_check(zox_tst_xr_reach_miss[i] < 0.01f, "reaching [%s] the hand's off its target [%.4f]", zox_tst_xr_reach_names[i], zox_tst_xr_reach_miss[i]);
    }
}

void zox_tsts_xr_reach(ecs* world) {
    zox_tst_add("xr_reach", zox_tst_xr_reach_ready, zox_tst_xr_reach_check, 120);
    zox_tst_set_setup("xr_reach", zox_tst_xr_sim_setup, 1);
}
