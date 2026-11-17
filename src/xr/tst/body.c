// tst: xr_body, in a game on the simulator the player's character is the body (fun/body.c)
//  - into a game as xr_play does, then: the head turned (the headset), the left stick held, walked in the room
//  - the head turn: the character's facing stays, its head bone turns, the camera (on it) looks where the head does
//  - the eyes ride the character: the view's eyes (the body, then the head) are at the camera every frame, and the
//    roam never takes the camera up (it did each frame, folding the head's turn into the body)
//  - the left stick walks the character; walking 0.3m in the room moves it 0.3m
//  - run: bash bsh/tst.sh xr_body
int zox_tst_xr_body_frames = 0;
float zox_tst_xr_body_worst_eyes = 0;       // the view's eyes from the camera, worst
float zox_tst_xr_body_facing_turn = 0;      // the character's facing, turned by the head (should stay)
float zox_tst_xr_body_look = 0;             // the camera's look on the head's (1 the same)
float zox_tst_xr_body_walked = 0;           // by the stick
float zox_tst_xr_body_room = 0;             // by the room
float4 zox_tst_xr_body_facing;
float3 zox_tst_xr_body_from;

byte zox_tst_xr_body_ready(ecs* world) {
    if (zox_tst_xr_play_stage < 3) {
        zox_tst_xr_play_ready(world);
        return 0;
    }
    const entity character = xr_body_character(world);
    if (!zox_valid(character) || !xr_body_head(world, xr_camera)) {
        zox_tst_waiting = "the xr camera on the character's head";
        return 0;
    }
    const int f = ++zox_tst_xr_body_frames;
    zox_tst_progress(f);
    zox_tst_waiting = "the head, stick and room";
    if (f > 60) {
        // the eyes in the view at the camera: the view's body, the head's centre on top (the sim's eyes straddle it)
        const float3 eyes = float4x4_get_position(float4x4_multiply(
            float4x4_transform(xr_sim_script_head_position, xr_head_now_rotation), xr_view_body));
        const float d = float3_distance(eyes, zox_getv(xr_camera, Position3D));
        // the stick and room steps move it a frame ahead of the hierarchy, so only while still
        if ((f < 150) && d > zox_tst_xr_body_worst_eyes) {
            zox_tst_xr_body_worst_eyes = d;
        }
    }
    if (f == 60) {
        zox_tst_xr_body_facing = zox_getv(character, Rotation3D);
    }
    if (f > 60 && f < 150) {
        xr_sim_head_euler.y = 0.8f;     // looked left
        xr_sim_head_euler.x = 0.3f;     // and up
    }
    if (f == 149) {
        const float4 now = zox_getv(character, Rotation3D);
        zox_tst_xr_body_facing_turn = 1 - fabsf(float4_dot(now, zox_tst_xr_body_facing));
        // the camera looks (-z) where the head does: the body, then the head
        const float3 look = float4_rotate_float3(zox_getv(xr_camera, Rotation3D), float3_backward);
        const float3 want = float4_rotate_float3(quaternion_rotate(xr_body_rotation(world, character), xr_head_now_rotation), float3_backward);
        zox_tst_xr_body_look = float3_dot(look, want);
        xr_sim_head_euler = float3_zero;
        zox_tst_xr_body_from = zox_getv(character, Position3D);
    }
    if (f >= 150 && f < 210) {
        xr_sim_script_left_stick = (float2) { 0, 1 };
    }
    if (f == 210) {
        xr_sim_script_left_stick = float2_zero;
        const float3 moved = float3_sub(zox_getv(character, Position3D), zox_tst_xr_body_from);
        zox_tst_xr_body_walked = sqrtf(moved.x * moved.x + moved.z * moved.z);
    }
    if (f == 240) {
        zox_tst_xr_body_from = zox_getv(character, Position3D);
        xr_sim_script_head_position = (float3) { 0.3f, 0, 0 };
    }
    if (f == 250) {
        const float3 moved = float3_sub(zox_getv(character, Position3D), zox_tst_xr_body_from);
        zox_tst_xr_body_room = sqrtf(moved.x * moved.x + moved.z * moved.z);
        xr_sim_script_head_position = float3_zero;
    }
    return f > 250;
}

void zox_tst_xr_body_check(ecs* world) {
    zox_log("[tst] xr_body: eyes off the camera [%.4f], facing turned [%.5f], look [%.4f], walked [%.3f], room [%.3f]",
        zox_tst_xr_body_worst_eyes, zox_tst_xr_body_facing_turn, zox_tst_xr_body_look, zox_tst_xr_body_walked, zox_tst_xr_body_room);
    zox_tst_check(zox_tst_xr_body_worst_eyes < 0.02f, "the eyes left the camera by [%f]", zox_tst_xr_body_worst_eyes);
    zox_tst_check(zox_tst_xr_body_facing_turn < 0.0001f, "the head's turn turned the body [%f]", zox_tst_xr_body_facing_turn);
    zox_tst_check(zox_tst_xr_body_look > 0.99f, "the camera doesn't look where the head does [%f]", zox_tst_xr_body_look);
    zox_tst_check(zox_tst_xr_body_walked > 0.3f, "the left stick didn't walk the character [%f]", zox_tst_xr_body_walked);
    zox_tst_check(fabsf(zox_tst_xr_body_room - 0.3f) < 0.05f, "walking 0.3m in the room moved it [%f]", zox_tst_xr_body_room);
}

void zox_tsts_xr_body(ecs* world) {
    zox_tst_add("xr_body", zox_tst_xr_body_ready, zox_tst_xr_body_check, 120);
    zox_tst_set_setup("xr_body", zox_tst_xr_sim_setup, 1);
}
