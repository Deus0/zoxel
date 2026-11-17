// XR culling: the xr camera's frustum (cameras: FieldOfViewAngles, CameraPlanes) holds both eyes' views, so the
//  chunks either eye sees are drawn (streaming/sys/frustum.c culls by it)
//  - each eye has its own off centre angles (a headset's outer sides wider) and sits a few cm off the camera: the
//      union of both, widened by a margin for that offset and the head turning between culling and drawing (the
//      eyes are located again just before they draw, openxr/render.c)
//  - it culled by the desktop's FieldOfView and the window's aspect, the eyes' edges cut off (tst xr_frustum)
#define xr_cull_margin 0.1f         // radians each side
#define xr_cull_angle_max 1.45f     // short of straight sideways (tan blows up)

static inline float4 xr_cull_angles(float4 a, float4 b) {
    return (float4) {
        float_max(-xr_cull_angle_max, float_min(a.x, b.x) - xr_cull_margin),
        float_min(xr_cull_angle_max, float_max(a.y, b.y) + xr_cull_margin),
        float_max(-xr_cull_angle_max, float_min(a.z, b.z) - xr_cull_margin),
        float_min(xr_cull_angle_max, float_max(a.w, b.w) + xr_cull_margin)
    };
}

// the eyes' angles onto the xr camera, for the next cull
static inline void xr_cull_set(ecs* world, float4 left_eye, float4 right_eye) {
    if (zox_valid(xr_camera) && zox_has(xr_camera, FieldOfViewAngles)) {
        zox_setv(xr_camera, FieldOfViewAngles, xr_cull_angles(left_eye, right_eye));
    }
}
