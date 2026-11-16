// The XR simulator: a headset on the desktop, both eyes side by side in the window (--xr-sim)
//  - turns zox_xr_active on, so it takes every branch a headset does (no player or ui cameras, the xr camera)
//  - the same per eye render (fun/eye_render.c) and free roam (fun/roam.c) as a headset, so what's right here
//      is right there, bar OpenXR itself (doc/plans/xr.md)
//  - the head: Q E turn it, R F tilt it; the sticks: W A S D the left (moves), arrows the right (turns)
//  - eyes 64mm apart, each with a headset-like fov, a little wider on its outer side
byte zox_xr_sim = 0;
#define xr_sim_eye_distance 0.064f
#define xr_sim_fov_up 0.8f              // radians, up and down
#define xr_sim_head_speed 1.5f          // radians a second

static float3 xr_sim_head_euler = { 0, 0, 0 };

void process_arguments_xr(ecs* world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (!strcmp(args[i], "--xr-sim")) {
            zox_xr_sim = 1;
            zox_xr_active = 1;
            zox_log("[XR] simulator on");
        }
    }
}

// the eye's four angles (left, right, down, up) for an eye of this aspect, outer side wider as a headset's
float4 xr_sim_eye_fov(byte eye, float aspect) {
    const float up = tanf(xr_sim_fov_up);
    const float side = up * aspect;
    const float shift = side * 0.1f * (eye == 0 ? 1 : -1);  // the left eye sees more to its left
    return (float4) { -atanf(side + shift), atanf(side - shift), -xr_sim_fov_up, xr_sim_fov_up };
}

// the eye in the head's space: the head turned, the eye to its side
float4x4 xr_sim_eye_matrix(byte eye, float4 head_rotation) {
    const float3 offset = { (eye == 0 ? -0.5f : 0.5f) * xr_sim_eye_distance, 0, 0 };
    return float4x4_transform(float4_rotate_float3(head_rotation, offset), head_rotation);
}

static inline float xr_sim_axis(const PhysicalButton* negative, const PhysicalButton* positive) {
    return (float) positive->is_pressed - (float) negative->is_pressed;
}

void xr_sim_render_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    if (!zox_xr_sim || !rendering || !zox_valid(main_app) || !zox_has(main_app, WindowSize)) {
        return;
    }
    const float delta = (float) zox_delta_time;
    float2 move = float2_zero;
    float2 turn = float2_zero;
    if (zox_valid(local_keyboard)) {
        const Keyboard* keyboard = zox_get(local_keyboard, Keyboard);
        move = (float2) { xr_sim_axis(&keyboard->a, &keyboard->d), xr_sim_axis(&keyboard->s, &keyboard->w) };
        turn = (float2) { xr_sim_axis(&keyboard->left, &keyboard->right), xr_sim_axis(&keyboard->down, &keyboard->up) };
        xr_sim_head_euler.y += xr_sim_axis(&keyboard->e, &keyboard->q) * xr_sim_head_speed * delta;
        xr_sim_head_euler.x += xr_sim_axis(&keyboard->f, &keyboard->r) * xr_sim_head_speed * delta;
    }
    const float4 head_rotation = quaternion_from_euler(xr_sim_head_euler);
    xr_roam_update(world, xr_camera, move, turn, head_rotation, delta);
    const float4x4 body = xr_roam_body_matrix();
    const int2 window = zox_getv(main_app, WindowSize);
    const float2 half = { window.x / 2, window.y };
    for (byte i = 0; i < 2; i++) {
        xr_render_eye(world, xr_sim_eye_matrix(i, head_rotation), xr_sim_eye_fov(i, half.x / half.y), body, (float2) { i * half.x, 0 }, half);
    }
    zox_sys_on_end();
} zoxd_system(xr_sim_render_system);

void zox_systems_xr_sim(ecs* world) {
    zox_system_1(xr_sim_render_system, zoxp_render, 0);
}
