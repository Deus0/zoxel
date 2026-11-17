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
float huds3_darkness = 0;               // eased to huds3_dark, rendering's render_darkness
double huds3_dark_held = 0;             // lit again only after this long unlit: the body settles on its spawn first
#define huds3_dark_fade_in 0.6f         // seconds to black
#define huds3_dark_fade_out 1.2f        // seconds from black
#define huds3_dark_hold 0.4f
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

// a menu as a 3D list ahead of the player's camera, a child of their canvas (found and closed as its 2D twin)
entity spawn_huds3_list(
    ecs* world,
    entity player,
    const char* header,
    byte header_font_size,
    byte font_size,
    const SpawnListElement* elements,
    byte count,
    entity* spawned)
{
    float3 position;
    float4 rotation;
    huds3_pose(world, player, huds3_distance, &position, &rotation);
    return spawn_list3(
        world,
        zox_get_link(world, player, CanvasLink),
        position,
        rotation,
        huds3_menu_height,
        header,
        header_font_size,
        font_size,
        elements,
        count,
        spawned);
}
