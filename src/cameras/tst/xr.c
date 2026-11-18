// tst: camera_projection_angles, a projection from four angles (cameras/fun/projection_angles.c, OpenXR's eyes)
//  - symmetric (45 degrees each side) and off centre (a headset's eye: wider to its outside): a point on each edge
//      of its view, 2 ahead, lands on that edge (x and y -1 and 1), the near and far planes at depth
//      -1 and 1
//  - y goes up: the up edge at 1 (it was flipped, the world drawn upside down)
//  - run: bash bsh/tst.sh camera_projection_angles
static inline float3 zox_tst_xr_ndc(float4x4 projection, float3 p) {
    const float4 clip = float4x4_multiply_float4(projection, (float4) { p.x, p.y, p.z, 1 });
    return (float3) { clip.x / clip.w, clip.y / clip.w, clip.z / clip.w };
}

void zox_tst_camera_projection_angles_check(ecs* world) {
    (void) world;
    const float near_plane = 0.05f, far_plane = 100, d = 2;
    // left, right, down, up
    const float4 angles[2] = { { -M_PI / 4, M_PI / 4, -M_PI / 4, M_PI / 4 }, { -0.9f, 0.7f, -0.8f, 0.75f } };
    const char* names[2] = { "symmetric", "off centre" };
    for (int i = 0; i < 2; i++) {
        const float4 a = angles[i];
        const float4x4 projection = camera_projection_from_angles(a, near_plane, far_plane);
        const float3 left = zox_tst_xr_ndc(projection, (float3) { tanf(a.x) * d, 0, -d });
        const float3 right = zox_tst_xr_ndc(projection, (float3) { tanf(a.y) * d, 0, -d });
        const float3 down = zox_tst_xr_ndc(projection, (float3) { 0, tanf(a.z) * d, -d });
        const float3 up = zox_tst_xr_ndc(projection, (float3) { 0, tanf(a.w) * d, -d });
        const float3 at_near = zox_tst_xr_ndc(projection, (float3) { 0, 0, -near_plane });
        const float3 at_far = zox_tst_xr_ndc(projection, (float3) { 0, 0, -far_plane });
        zox_log("[tst] [metric] camera_projection_angles %s: left [%.3f] right [%.3f] down [%.3f] up [%.3f] near [%.3f] far [%.3f]",
            names[i], left.x, right.x, down.y, up.y, at_near.z, at_far.z);
        zox_tst_check(fabsf(left.x + 1) < 1e-4f && fabsf(right.x - 1) < 1e-4f, "%s: the left and right edges land at [%.3f] [%.3f], not -1 and 1",
            names[i], left.x, right.x);
        zox_tst_check(fabsf(down.y + 1) < 1e-4f && fabsf(up.y - 1) < 1e-4f,
            "%s: the down and up edges land at [%.3f] [%.3f], not -1 and 1", names[i], down.y, up.y);
        zox_tst_check(fabsf(at_near.z + 1) < 1e-3f && fabsf(at_far.z - 1) < 1e-3f, "%s: near and far depths [%.3f] [%.3f], not -1 and 1",
            names[i], at_near.z, at_far.z);
    }
}

// tst: camera_free_roam, a camera flown by two sticks (cameras/fun/free_roam.c, XR's free roam)
//  - the left stick forward a second moves free_roam_move_speed along -z, right along +x; the head turned left a
//      quarter, forward goes its way (-x)
//  - the right stick held right a second yaws -free_roam_turn_speed
//  - frame rate: a second at 30, 60 and 120 steps a second ends at the same place (turning and moving at once)
//  - run: bash bsh/tst.sh camera_free_roam
static inline float3 zox_tst_roam_second(float2 move, float2 turn, float4 head, int steps) {
    free_roam roam = { 0 };
    for (int i = 0; i < steps; i++) {
        free_roam_step(&roam, move, turn, head, 1.0f / steps);
    }
    return roam.position;
}

void zox_tst_camera_free_roam_check(ecs* world) {
    (void) world;
    const float4 ahead = quaternion_identity;
    const float3 forward = zox_tst_roam_second((float2) { 0, 1 }, float2_zero, ahead, 60);
    const float3 right = zox_tst_roam_second((float2) { 1, 0 }, float2_zero, ahead, 60);
    const float3 head_left = zox_tst_roam_second((float2) { 0, 1 }, float2_zero, quaternion_from_axis_angle(float3_up, M_PI / 2), 60);
    zox_log("[tst] [metric] camera_free_roam forward [%.3f %.3f %.3f] right [%.3f %.3f %.3f] head turned left [%.3f %.3f %.3f]",
        forward.x, forward.y, forward.z, right.x, right.y, right.z, head_left.x, head_left.y, head_left.z);
    const float s = free_roam_move_speed;
    zox_tst_check(float3_length(float3_sub(forward, (float3) { 0, 0, -s })) < 1e-3f, "forward a second went [%.3f %.3f %.3f], not [0 0 %.1f]",
        forward.x, forward.y, forward.z, -s);
    zox_tst_check(float3_length(float3_sub(right, (float3) { s, 0, 0 })) < 1e-3f, "right a second went [%.3f %.3f %.3f], not [%.1f 0 0]",
        right.x, right.y, right.z, s);
    zox_tst_check(float3_length(float3_sub(head_left, (float3) { -s, 0, 0 })) < 1e-3f,
        "forward with the head turned left went [%.3f %.3f %.3f], not its way [%.1f 0 0]", head_left.x, head_left.y, head_left.z, -s);
    // turning
    free_roam roam = { 0 };
    for (int i = 0; i < 60; i++) {
        free_roam_step(&roam, float2_zero, (float2) { 1, 0 }, ahead, 1.0f / 60);
    }
    zox_tst_check(fabsf(roam.euler.y + free_roam_turn_speed) < 1e-3f, "held right a second it yawed [%.3f], not [%.3f]", roam.euler.y, -free_roam_turn_speed);
    // the same second at any frame rate, moving straight (turning while moving curves, it can't match exactly)
    const float3 at_30 = zox_tst_roam_second((float2) { 0.5f, 1 }, float2_zero, ahead, 30);
    const float3 at_120 = zox_tst_roam_second((float2) { 0.5f, 1 }, float2_zero, ahead, 120);
    zox_tst_check(float3_length(float3_sub(at_30, at_120)) < 1e-3f, "a second at 30 and 120 frames ends [%.4f] apart",
        float3_length(float3_sub(at_30, at_120)));
    // turning while moving: close at any frame rate (the path curves, finer steps follow it better)
    const float3 curve_30 = zox_tst_roam_second((float2) { 0, 1 }, (float2) { 0.5f, 0 }, ahead, 30);
    const float3 curve_120 = zox_tst_roam_second((float2) { 0, 1 }, (float2) { 0.5f, 0 }, ahead, 120);
    zox_log("[tst] [metric] camera_free_roam turning while moving, 30 and 120 frames end [%.4f] apart",
        float3_length(float3_sub(curve_30, curve_120)));
    zox_tst_check(float3_length(float3_sub(curve_30, curve_120)) < 0.05f, "turning while moving, 30 and 120 frames end [%.4f] apart",
        float3_length(float3_sub(curve_30, curve_120)));
}

// tst: camera_xr_eyes, an eye placed by the camera (xr/fun/eye_render.c: the eye's pose, then the camera's)
//  - eyes 64mm apart, the camera moved and turned: each eye is the camera's position plus its offset turned by the
//      camera, both level and the distance apart kept, along the camera's right
//  - run: bash bsh/tst.sh camera_xr_eyes
void zox_tst_camera_xr_eyes_check(ecs* world) {
    (void) world;
    const float3 camera_position = { 10, 2, -5 };
    const float4 camera_rotation = quaternion_from_axis_angle(float3_up, 0.7f);
    const float4x4 camera = float4x4_transform(camera_position, camera_rotation);
    const float3 offsets[2] = { { -0.032f, 0, 0 }, { 0.032f, 0, 0 } };
    float3 eyes[2];
    for (int i = 0; i < 2; i++) {
        const float4x4 eye = float4x4_transform(offsets[i], quaternion_identity);
        eyes[i] = float4x4_get_position(float4x4_multiply(eye, camera));
        const float3 expected = float3_add(camera_position, float4_rotate_float3(camera_rotation, offsets[i]));
        zox_tst_check(float3_length(float3_sub(eyes[i], expected)) < 1e-4f, "eye [%i] at [%.3f %.3f %.3f], not [%.3f %.3f %.3f]",
            i, eyes[i].x, eyes[i].y, eyes[i].z, expected.x, expected.y, expected.z);
    }
    const float3 apart = float3_sub(eyes[1], eyes[0]);
    const float3 camera_right = float4_rotate_float3(camera_rotation, (float3) { 1, 0, 0 });
    zox_tst_check(fabsf(float3_length(apart) - 0.064f) < 1e-4f && fabsf(apart.y) < 1e-5f, "the eyes are [%.4f] apart, [%.4f] off level",
        float3_length(apart), apart.y);
    zox_tst_check(float3_dot(float3_normalize(apart), camera_right) > 0.9999f, "the eyes aren't apart along the camera's right");
}
