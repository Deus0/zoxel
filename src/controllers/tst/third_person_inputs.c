// tst: third_person_inputs, the third person camera and walk driven by fake devices, as a player plays them (play
//  tests 2026-11-27: the mouse's up and down and touch's look were inverted)
//  - the test level, the camera switched to third person (the orbit camera, controllers3/sys/orbit.c)
//  - a click (the mouse's device mode, as a player clicking into the game), then the fake mouse moved up: the camera lowers (it looks up); moved right: the view turns right
//  - the fake keyboard's W held: the character walks away from the camera
//  - a tap switches to touch; a finger on the right half (the look joystick) dragged right: the view turns right;
//    dragged up: the camera rises
//  - turning right: the camera's flat forward turns clockwise seen from above, its cross with the one before has a
//    negative y
//  - run: bash bsh/tst.sh third_person_inputs
int zox_tst_tpi_stage = 0;
int zox_tst_tpi_frames = 0;
float3 zox_tst_tpi_forward;     // the camera's flat forward, as a step began
float zox_tst_tpi_height;       // the camera's height over its target, as a step began
float3 zox_tst_tpi_start;       // the character's position, as the walk began
double zox_tst_tpi_time = 0;

static inline float3 zox_tst_tpi_camera_forward(ecs* world, entity camera) {
    const float3 f = float4_rotate_float3(zox_getv(camera, Rotation3D), (float3) { 0, 0, -1 });
    return float3_normalize((float3) { f.x, 0, f.z });
}

static inline float zox_tst_tpi_camera_height(ecs* world, entity camera, entity character) {
    return zox_getv(camera, Position3D).y - zox_getv(character, Position3D).y;
}

// how far the view turned right since the step began (radians, negative is left)
static inline float zox_tst_tpi_turned_right(ecs* world, entity camera) {
    const float3 now = zox_tst_tpi_camera_forward(world, camera);
    const float cross_y = zox_tst_tpi_forward.z * now.x - zox_tst_tpi_forward.x * now.z;
    return -asinf(clampf(cross_y, -1, 1));
}

static inline void zox_tst_tpi_key_w(ecs* world, byte held) {
    entity devices[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, zox_players[0], devices, zox_children_capacity, zox_id(Device));
    for (uint i = 0; i < length; i++) {
        if (zox_has(devices[i], Keyboard)) {
            Keyboard keys = { 0 };
            keys.w.is_pressed = held;
            zox_set_ptr(devices[i], Keyboard, keys);
        }
    }
}

static inline void zox_tst_tpi_finger(float x, float y, byte down) {
    zox_touch_fake = 1;
    zox_touch_fake_count = down ? 1 : 0;
    zox_touch_fake_fingers[0] = (zox_touch_finger) { 0, x, y };
}

#define zox_tst_tpi_next(stage) { zox_tst_tpi_stage = stage; zox_tst_tpi_frames = 0; return 0; }

byte zox_tst_third_person_inputs_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity camera = zox_get_link(world, zox_players[0], CameraLink);
    if (!zox_valid(character) || !zox_valid(camera)) {
        zox_tst_waiting = "the player's character and camera";
        return 0;
    }
    zox_tst_tpi_frames++;
    zox_tst_progress(zox_tst_tpi_stage * 1000 + zox_tst_tpi_frames);
    const int2 middle = { 120, 120 };     // the fake mouse starts here (inside a test's window), moves from it
    if (zox_tst_tpi_stage == 0) {
        zox_tst_waiting = "the orbit camera";
        if (zox_tst_tpi_frames == 1) {
            set_camera_mode(world, camera, zox_camera_state_third_person);
        }
        zox_tst_mouse(middle, zox_tst_tpi_frames >= 5 && zox_tst_tpi_frames < 9);     // a click: the mouse's mode
        if (zox_tst_tpi_frames < 30) {
            return 0;
        }
        zox_tst_check(zox_getv(camera, CameraState) == zox_camera_state_third_person, "the camera isn't third person");
        zox_tst_tpi_height = zox_tst_tpi_camera_height(world, camera, character);
        zox_tst_tpi_next(1);
    }
    if (zox_tst_tpi_stage == 1) {
        // the mouse up a little each frame (pointer space: y up)
        zox_tst_waiting = "the mouse moved up";
        zox_tst_mouse((int2) { middle.x, middle.y + zox_tst_tpi_frames * 4 }, 0);
        if (zox_tst_tpi_frames < 20) {
            return 0;
        }
        const float height = zox_tst_tpi_camera_height(world, camera, character);
        zox_tst_check(height < zox_tst_tpi_height - 0.05f, "the mouse up: the camera went from [%.2f] to [%.2f] over "
            "the character, not lower (it looks up)", zox_tst_tpi_height, height);
        zox_tst_tpi_forward = zox_tst_tpi_camera_forward(world, camera);
        zox_tst_tpi_next(2);
    }
    if (zox_tst_tpi_stage == 2) {
        zox_tst_waiting = "the mouse moved right";
        zox_tst_mouse((int2) { middle.x + zox_tst_tpi_frames * 4, middle.y + 80 }, 0);
        if (zox_tst_tpi_frames < 20) {
            return 0;
        }
        const float turned = zox_tst_tpi_turned_right(world, camera);
        zox_tst_check(turned > 0.05f, "the mouse right turned the view [%.2f] radians right", turned);
        zox_tst_mouse_end();
        zox_tst_tpi_start = zox_getv(character, Position3D);
        zox_tst_tpi_forward = zox_tst_tpi_camera_forward(world, camera);
        zox_tst_tpi_time = zox_current_time;
        zox_tst_tpi_next(3);
    }
    if (zox_tst_tpi_stage == 3) {
        zox_tst_waiting = "W held";
        zox_tst_tpi_key_w(world, 1);
        if (zox_current_time - zox_tst_tpi_time < 1.5) {
            return 0;
        }
        zox_tst_tpi_key_w(world, 0);
        const float3 at = zox_getv(character, Position3D);
        const float3 walked = { at.x - zox_tst_tpi_start.x, 0, at.z - zox_tst_tpi_start.z };
        const float length = float3_length(walked);
        const float along = length > 0 ? float3_dot(float3_scale(walked, 1 / length), zox_tst_tpi_forward) : 0;
        zox_tst_check(length > 0.5f && along > 0.9f, "W walked [%.2f] m, [%.2f] of it away from the camera", length,
            along);
        zox_tst_tpi_next(4);
    }
    if (zox_tst_tpi_stage == 4) {
        // a tap: touch mode
        zox_tst_waiting = "a tap";
        zox_tst_tpi_finger(0.5f, 0.3f, zox_tst_tpi_frames < 5);
        if (zox_tst_tpi_frames < 20) {
            return 0;
        }
        zox_tst_check(zox_getv(zox_players[0], DeviceMode) == zox_device_mode_touchscreen, "a tap didn't switch to touch");
        zox_tst_tpi_next(5);
    }
    if (zox_tst_tpi_stage == 5) {
        // the look joystick (the right half, sdl's y from the top) dragged right, held
        zox_tst_waiting = "the look joystick dragged right";
        if (zox_tst_tpi_frames == 6) {
            zox_tst_tpi_forward = zox_tst_tpi_camera_forward(world, camera);
        }
        zox_tst_tpi_finger(zox_tst_tpi_frames < 5 ? 0.75f : 0.85f, 0.6f, 1);
        if (zox_tst_tpi_frames < 40) {
            return 0;
        }
        const float turned = zox_tst_tpi_turned_right(world, camera);
        zox_tst_check(turned > 0.05f, "touch dragged right turned the view [%.2f] radians right", turned);
        zox_tst_tpi_next(6);
    }
    if (zox_tst_tpi_stage == 6) {
        zox_tst_waiting = "letting go";
        zox_tst_tpi_finger(0.85f, 0.6f, 0);
        if (zox_tst_tpi_frames < 10) {
            return 0;
        }
        zox_tst_tpi_next(7);
    }
    if (zox_tst_tpi_stage == 7) {
        // dragged up (sdl's y down the screen, so less)
        zox_tst_waiting = "the look joystick dragged up";
        if (zox_tst_tpi_frames == 6) {
            zox_tst_tpi_height = zox_tst_tpi_camera_height(world, camera, character);
        }
        zox_tst_tpi_finger(0.75f, zox_tst_tpi_frames < 5 ? 0.6f : 0.5f, 1);
        if (zox_tst_tpi_frames < 40) {
            return 0;
        }
        const float height = zox_tst_tpi_camera_height(world, camera, character);
        zox_tst_check(height > zox_tst_tpi_height + 0.05f, "touch dragged up: the camera went from [%.2f] to [%.2f] over "
            "the character, not higher", zox_tst_tpi_height, height);
        zox_tst_tpi_finger(0.75f, 0.5f, 0);
        zox_touch_fake = 0;
        return 1;
    }
    return 0;
}

void zox_tst_third_person_inputs_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_tpi_stage == 7, "it stopped at stage [%i]", zox_tst_tpi_stage);
}
