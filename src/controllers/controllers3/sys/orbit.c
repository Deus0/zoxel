// The orbit camera (third person): not a child of its target (CameraTarget), it orbits it
//  - the mouse (locked) or the right stick (a gamepad's, touch's) turns it round (orbit_look_input) (Euler: pitch x, yaw y, in radians), the wheel zooms
//  - it looks at the target's head height from its OrbitDistance away (orbit_distance to start), eased there (orbit_follow a second)
//  - on a poly terrain (its target's TerrainLink) it stays over the ground
//  - the character walks along its facing (movement.c), not turned by the mouse (rotation.c)
float orbit_distance_min = 2;
float orbit_distance_max = 20;
float orbit_height = 1.2f;          // looks at this over the target's position
float orbit_pitch_min = -0.3f;      // radians: under the target a little
float orbit_pitch_max = 1.3f;       // nearly overhead
float orbit_follow = 12;            // how fast it eases to where it should be (a second)
float orbit_mouse = 0.004f;
float orbit_stick = 2.5f;           // radians a second at full stick
float orbit_wheel = 0.8f;           // metres a wheel step

// a look's turn of the orbit camera: x yaw, y pitch (up raises it over its target), from a mouse's delta (y up) or
//  a stick's push (gamepad and touch: x is left, y up) this frame (play tests 2026-11-27: both were inverted)
//  - mouse: right turns it right, up lowers it (it looks up, as first person)
//  - stick: left turns it left, up raises it (a touch joystick drags the camera round its target)
static inline float2 orbit_look_input(float2 mouse_delta, float2 stick, float dt) {
    return (float2) {
        -mouse_delta.x * orbit_mouse + stick.x * orbit_stick * dt,
        -mouse_delta.y * orbit_mouse + stick.y * orbit_stick * dt };
}

// the device inputs that turn and zoom it: mouse delta (when locked), right stick, the wheel
static inline float3 orbit_read_input(ecs* world, entity player) {
    float3 input = float3_zero;     // x yaw, y pitch, z zoom
    entity devices[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, player, devices, zox_children_capacity, zox_id(Device));
    for (uint j = 0; j < length; j++) {
        if (zox_has(devices[j], Disabled)) {
            continue;
        }
        entity children[zox_children_capacity];
        const uint count = zox_get_children(world, devices[j], children, zox_children_capacity);
        for (uint k = 0; k < count; k++) {
            const entity e3 = children[k];
            if (zox_has(e3, Disabled) || zox_has(e3, Finger)) {
                continue;
            }
            if (zox_has(e3, ZevicePointerDelta) && zox_has(e3, MousePointer)) {
                const float2 look = orbit_look_input(int2_to_float2(zox_getv(e3, ZevicePointerDelta)), float2_zero, 0);
                input.x += look.x;
                input.y += look.y;
            }
            if (zox_has(e3, ZeviceWheel)) {
                input.z -= zox_getv(e3, ZeviceWheel).y * orbit_wheel;
            }
            if (zox_has(e3, ZeviceStick) && zox_getv(e3, DeviceButtonType) == zox_device_stick_right) {
                const float2 look = orbit_look_input(float2_zero, zox_getv(e3, ZeviceStick), (float) zox_delta_time);
                input.x += look.x;
                input.y += look.y;
            }
        }
    }
    return input;
}

// the lowest a camera may be over a poly terrain at a point (world space), or -infinity off it
static inline float orbit_ground(ecs* world, entity target, float3 point) {
    const entity terrain = zox_get_link(world, target, TerrainLink);
    if (!zox_valid(terrain) || !zox_has(terrain, PolyTerrain)) {
        return -INFINITY;
    }
    const PolyHeights* heights = zox_get(terrain, PolyHeights);
    const float3 local = float3_sub(point, zox_getv(terrain, Position3D));
    return poly_height_at(heights->value, zox_getv(terrain, PolyCells), zox_getv(terrain, Size2), local.x, local.z) + 0.4f;
}

void orbit_camera_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        const entity camera = zox_get_link(world, e, CameraLink);
        if (state->value != zox_player_state_playing || !zox_valid(camera) ||
            zox_getv(camera, CameraState) != zox_camera_state_third_person) {
            continue;
        }
        const entity target = zox_get_link(world, camera, CameraTarget);
        if (!zox_valid(target) || !zox_has(camera, OrbitDistance)) {
            continue;
        }
        const float3 input = orbit_read_input(world, e);
        float3 euler = zox_getv(camera, Euler);
        euler.y += input.x;
        euler.x = clampf(euler.x + input.y, orbit_pitch_min, orbit_pitch_max);
        const float distance = clampf(zox_getv(camera, OrbitDistance) + input.z, orbit_distance_min, orbit_distance_max);
        // behind the target: from its head along the camera's back (+z turned by yaw, raised by pitch)
        const float3 look = float3_add(zox_getv(target, Position3D), (float3) { 0, orbit_height, 0 });
        const float3 back = { sinf(euler.y) * cosf(euler.x), sinf(euler.x), cosf(euler.y) * cosf(euler.x) };
        float3 wanted = float3_add(look, float3_scale(back, distance));
        wanted.y = fmaxf(wanted.y, orbit_ground(world, target, wanted));
        const float ease = fminf(1, orbit_follow * zox_delta_time);
        const float3 position = float3_lerp(zox_getv(camera, Position3D), wanted, ease);
        zox_setv(camera, OrbitDistance, distance);
        zox_setv(camera, Euler, euler);
        zox_setv(camera, Position3D, position);
        zox_setv(camera, Rotation3D, camera_look_at(position, look));
    }
    zox_sys_on_end();
} zoxd_system(orbit_camera_system);
