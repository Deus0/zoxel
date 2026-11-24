// 3D huds (zox_huds3D, on while XR is): menus as panels in the world, ahead of the player's camera
//  - before the camera's spawned (huds boot before xr's camera), the main menu camera's spot, where it spawns
#define huds3_distance 1.2f        // metres ahead
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

// a 3D hud's root ahead of the player's head, the canvas's child (found and deleted as a 2D one), what's in it spawned
//  by the same code as in 2D (the 3D twins, ui_prefab_for): a canvas3 of size pixels, height metres tall
entity huds3_spawn_root(ecs* world, entity player, entity canvas, int2 size, float height) {
    float3 position;
    float4 rotation;
    huds3_pose(world, player, huds3_distance, &position, &rotation);
    const entity e = spawn_canvas3(world, prefab_canvas3, position, height, size);
    zox_setv(e, Rotation3D, rotation);
    zox_set_parent(world, e, canvas);
    zox_link(world, e, PlayerLink, player);
    return e;
}

// a 3D window ahead of its canvas's player (spawn_window on 3D huds: ui_window3_pose)
void huds3_window_pose(ecs* world, entity canvas, float3* position, float4* rotation) {
    huds3_pose(world, zox_get_link(world, canvas, PlayerLink), huds3_distance, position, rotation);
}

// loading: the eyes close (each player's camera's CameraLids: lids from the top and bottom, in 2D and a headset), the
//  "Loading" label over them (2D, or 3D ahead of the head, following it: huds3_follow_system); they open once loaded
//  and the camera's on the body (load_dark_hold), load_lids_system
//  - the fog fading the world to black didn't blacken a headset's view (play test)
byte load_dark = 0;                     // loading: the eyes closing or shut
byte load_dark_hold = 0;                // kept closed: the camera isn't on the body yet (xr: it teleports there, a pop)
byte load_lids = 0;                     // loading drives the lids (closing, or opening after): sleeping's are its own
float huds3_label_fade = 0;             // the 3D label, 0 unseen to 1 lit, once shut
double load_dark_held = 0;              // opened only after this long unheld: the body settles on its spawn first
#define load_dark_close 0.45f           // seconds to close
#define load_dark_open 0.6f             // seconds to open
#define load_dark_hold_time 0.4f
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

