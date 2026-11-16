// A projection from four angles (radians from straight ahead, left and down negative): OpenXR gives each eye
//  its own, not one fov, an eye's view isn't centred
//  - x: the left and right edges map to -1 and 1 (tst camera_projection_angles)
//  - y: as written the up edge maps to -1 and down to 1 (2 / (tan down - tan up)); OpenXR's GL samples have it the
//      other way. xr/sys/render.c sets glFrontFace(GL_CW) ("XR triangle direction"), which a flipped y needs: the
//      two may cancel, check on a headset before changing either
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
            2.0f / (d - u),
            0,
            0
        },
        .z = {
            (r + l) / (r - l),
            (d + u) / (d - u),
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
