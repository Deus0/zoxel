// 3D huds (zox_huds3D, on while XR is): menus as panels in the world, ahead of the player's camera
//  - before the camera's spawned (huds boot before xr's camera), the main menu camera's spot, where it spawns
#define huds3_distance 1.2f        // metres ahead
#define huds3_menu_height 0.5f     // a list's height in metres
#define huds3_label_height 0.06f

void huds3_pose(ecs* world, entity player, float distance, float3* position, float4* rotation) {
    *position = main_menu_camera_position;
    *rotation = main_menu_camera_rotation;
    const entity camera = zox_get_link(world, player, CameraLink);
    if (zox_valid(camera) && zox_has(camera, Position3D) && zox_has(camera, Rotation3D)) {
        *position = zox_getv(camera, Position3D);
        *rotation = zox_getv(camera, Rotation3D);
    }
    *position = move_along_direction(*position, *rotation, -distance);
}

// loading on 3D huds: the view darkened to black (a headset's eyes draw only the 3D uis, xr/fun/eye_render.c) and a
//  3D label "Loading" ahead of the head, following it (huds3_follow_system); 2D loading fades its canvas instead
byte huds3_dark = 0;
#define huds3_follow_speed 4.0f     // per second, the label catching up with where the head looks

// eases e towards its pose ahead of its player's camera
void huds3_follow(ecs* world, entity e, float delta) {
    const entity player = zox_get_link(world, e, PlayerLink);
    if (!zox_valid(player)) {
        return;
    }
    float3 position;
    float4 rotation;
    huds3_pose(world, player, huds3_distance, &position, &rotation);
    const float t = delta <= 0 ? 1 : float_min(1, delta * huds3_follow_speed);
    zox_setv(e, Position3D, float3_lerp(zox_getv(e, Position3D), position, t));
    zox_setv(e, Rotation3D, quaternion_slerp(zox_getv(e, Rotation3D), rotation, t));
}
