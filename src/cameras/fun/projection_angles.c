// A projection from four angles (radians from straight ahead, left and down negative): OpenXR gives each eye
//  its own, not one fov, an eye's view isn't centred
//  - x: the left and right edges map to -1 and 1 (tst camera_projection_angles)
//  - y: the up edge maps to 1 and down to -1, as OpenXR's GL samples and the engine's cameras (it was flipped, drawn
//      upside down with GL_CW to keep the faces: tst xr_sim_eyes, the ground under the horizon)
//  - near and far are distances, depth maps near to -1 and far to 1
static inline float4x4 camera_projection_from_angles(
    float4 fov,
    float n,
    float z
) {
    const float l = tanf(fov.x);
    const float r = tanf(fov.y);
    const float d = tanf(fov.z);
    const float u = tanf(fov.w);
    return (float4x4){
        .x = {
            2.0f / (r - l),
            0,
            0,
            0
        },
        .y = {
            0,
            2.0f / (u - d),
            0,
            0
        },
        .z = {
            (r + l) / (r - l),
            (u + d) / (u - d),
            -(z + n) / (z - n),
            -1.0f
        },
        .w = {
            0,
            0,
            -(2.0f * z * n) / (z - n),
            0
        }
    };
}
