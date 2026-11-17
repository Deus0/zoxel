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

// loading on 3D huds: the eyes close (dark lids from the top and bottom, a headset's eyes: xr/fun/eye_render.c), then
//  only a 3D label "Loading" is drawn, fading in ahead of the head and following it (huds3_follow_system); the eyes
//  open once loaded and the camera's on the body (huds3_dark_hold); 2D loading fades its canvas instead
//  - the fog fading the world to black didn't blacken a headset's view (play test)
byte huds3_dark = 0;
byte huds3_dark_hold = 0;               // kept closed: the camera isn't on the body yet (xr: it teleports there, a pop)
float huds3_darkness = 0;               // the eyes closing, 0 open to 1 shut (huds3_dark_system)
float huds3_label_fade = 0;             // the label, 0 unseen to 1 lit, once shut
double huds3_dark_held = 0;             // opened only after this long unheld: the body settles on its spawn first
#define huds3_dark_close 0.45f          // seconds to close
#define huds3_dark_open 0.6f            // seconds to open
#define huds3_dark_hold_time 0.4f
#define huds3_label_fade_time 0.4f
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
