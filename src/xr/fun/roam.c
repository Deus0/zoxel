// The XR camera flown in free roam: the sticks move and turn it, the headset turns the head on top
//  - free_roam_step (cameras/fun/free_roam.c): the left stick moves where the head looks, the right turns the body
//  - the eyes render from the roam's transform (its body), each eye's located pose on top (render.c)
//  - the camera entity follows, at the head's place and facing, so streaming and sound follow the headset
static free_roam xr_roam;
static byte xr_roam_started = 0;

static inline float4x4 xr_roam_body_matrix() {
    return float4x4_transform(xr_roam.position, free_roam_rotation(&xr_roam));
}

static void xr_roam_update(ecs* world, entity camera, float delta) {
    if (!xr_roam_started && zox_valid(camera)) {
        xr_roam.position = zox_getv(camera, Position3D);
        xr_roam.euler = quaternion_to_euler(zox_getv(camera, Rotation3D));
        xr_roam_started = 1;
    }
    float2 move = float2_zero;
    float2 turn = float2_zero;
    if (xr_update_input()) {
        move = xr_get_left_stick();
        turn = xr_get_right_stick();
    }
    free_roam_step(&xr_roam, move, turn, xr_head_rotation, delta);
    if (zox_valid(camera)) {
        const float4 body = free_roam_rotation(&xr_roam);
        zox_setv(camera, Position3D, xr_roam.position);
        zox_setv(camera, Rotation3D, quaternion_rotate(body, xr_head_rotation));
    }
}
