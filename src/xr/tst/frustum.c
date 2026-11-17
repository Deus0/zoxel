// tst: xr_frustum, the xr camera's culling frustum (CameraPlanes, what chunk_frustum_system culls by) holds what both
//  eyes see: in a game on the simulator, the head turned about, each eye's view edges (its own off centre angles,
//  from where it is) at a few distances are inside the camera's planes; it culled by the desktop's fov and the
//  window's aspect, not the eyes'
//  - run: bash bsh/tst.sh xr_frustum
int zox_tst_xr_frustum_frames = 0;
int zox_tst_xr_frustum_checked = 0;
float zox_tst_xr_frustum_worst = 0;     // an eye's view edge outside a plane, worst, over its distance (0 inside)

// how far p is outside the planes (0 inside), as is_sphere_in_frustum measures
static inline float zox_tst_xr_frustum_outside(const plane* planes, float3 p) {
    float worst = 0;
    for (int i = 0; i < zox_camera_planes; i++) {
        const float d = float3_dot(planes[i].normal, p) - planes[i].distance;
        worst = float_max(worst, -d);
    }
    return worst;
}

byte zox_tst_xr_frustum_ready(ecs* world) {
    if (zox_tst_xr_play_stage < 3) {
        zox_tst_xr_play_ready(world);
        return 0;
    }
    zox_tst_waiting = "the head turned about";
    const int f = ++zox_tst_xr_frustum_frames;
    zox_tst_progress(f);
    // turned and tilted, a different way every 30 frames
    const float3 turns[] = { { 0, 0, 0 }, { -0.5f, 1.2f, 0 }, { 0.6f, -2.0f, 0.3f }, { 0, 3.0f, -0.2f } };
    xr_sim_head_euler = turns[(f / 30) % 4];
    if (f % 30 < 5) {
        return 0;   // the camera settles on the new turn
    }
    const plane* planes = zox_get(xr_camera, CameraPlanes)->value;
    const float4 head_rotation = quaternion_from_euler(xr_sim_head_euler);
    const int2 window = zox_getv(main_app, WindowSize);
    const float aspect = (window.x / 2) / (float) window.y;
    const float distances[] = { 1, 8, 40 };
    for (byte i = 0; i < 2; i++) {
        float4x4 eye = xr_sim_eye_matrix(i, head_rotation);
        eye.w.x *= xr_view_scale;
        eye.w.y *= xr_view_scale;
        eye.w.z *= xr_view_scale;
        const float4x4 world_eye = float4x4_multiply(eye, xr_view_body);
        const float4 fov = xr_sim_eye_fov(i, aspect);
        for (int k = 0; k < 3; k++) {
            const float d = distances[k];
            for (int c = 0; c < 4; c++) {
                const float x = tanf(c % 2 ? fov.y : fov.x) * d;
                const float y = tanf(c / 2 ? fov.w : fov.z) * d;
                const float4 p = float4x4_multiply_float4(world_eye, (float4) { x, y, -d, 1 });
                const float outside = zox_tst_xr_frustum_outside(planes, (float3) { p.x, p.y, p.z }) / d;
                zox_tst_xr_frustum_worst = float_max(zox_tst_xr_frustum_worst, outside);
            }
        }
    }
    zox_tst_xr_frustum_checked++;
    return f >= 120;
}

void zox_tst_xr_frustum_check(ecs* world) {
    zox_log("[tst] xr_frustum: [%i] frames, the eyes' view edges outside the camera's frustum, worst [%.4f] of distance",
        zox_tst_xr_frustum_checked, zox_tst_xr_frustum_worst);
    zox_tst_check(zox_tst_xr_frustum_checked > 60, "checked [%i] frames", zox_tst_xr_frustum_checked);
    zox_tst_check(zox_tst_xr_frustum_worst < 0.01f, "an eye sees [%.3f] of its distance past the culling frustum",
        zox_tst_xr_frustum_worst);
}

void zox_tsts_xr_frustum(ecs* world) {
    zox_tst_add("xr_frustum", zox_tst_xr_frustum_ready, zox_tst_xr_frustum_check, 120);
    zox_tst_set_setup("xr_frustum", zox_tst_xr_sim_setup, 1);
}
