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
