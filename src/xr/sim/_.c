// The XR simulator: a headset on the desktop, both eyes side by side in the window (--xr-sim), an xr_runtime
//  (fun/runtime.c) as OpenXR's is
//  - turns zox_xr_active on, so it takes every branch a headset does (no player or ui cameras, the xr camera)
//  - the same per eye render (fun/eye_render.c) and free roam (fun/roam.c) as a headset, so what's right here
//      is right there, bar OpenXR itself (doc/plans/xr.md)
//  - the head: Q E turn it, R F tilt it; the sticks: W A S D the left (moves), arrows the right (turns)
//  - the buttons: space or enter A, backspace B, escape menu; the hands held ahead of the head, either side
//  - the keyboard and mouse feed the controllers (DeviceFeeder), they're not a device mode of their own
//  - eyes 64mm apart, each with a headset-like fov, a little wider on its outer side
byte zox_xr_sim = 0;
#define xr_sim_eye_distance 0.064f
#define xr_sim_fov_up 0.8f              // radians, up and down
#define xr_sim_head_speed 1.5f          // radians a second
#define xr_sim_hand_side 0.2f           // metres, the hands from the head
#define xr_sim_hand_drop 0.3f
#define xr_sim_hand_ahead 0.4f

static float3 xr_sim_head_euler = { 0, 0, 0 };
// scripted presses (tests), held on top of the keys: the right hand's A and stick, the left's stick, the head's
//  place in the room (walking in it) and turn
byte xr_sim_script_a = 0;
byte xr_sim_script_start = 0;    // the left hand's menu button
float2 xr_sim_script_stick = { 0, 0 };
float2 xr_sim_script_left_stick = { 0, 0 };
float3 xr_sim_script_head_position = { 0, 0, 0 };
float3 xr_sim_script_hands[2];          // the hands' places in the room (tests), while xr_sim_script_hands_on
byte xr_sim_script_hands_on = 0;
float3 xr_sim_script_aims[2];           // the hands' aims (tests), apart from their grips, while xr_sim_script_aims_on
float4 xr_sim_script_aim_rotations[2] = { { 0, 0, 0, 1 }, { 0, 0, 0, 1 } };
byte xr_sim_script_aims_on = 0;

void process_arguments_xr(ecs* world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (!strcmp(args[i], "--xr-sim")) {
            zox_xr_sim = 1;
            zox_xr_active = 1;
            zox_huds3D = 1;
            zox_log("[XR] simulator on");
        }
    }
}

// the eye's four angles (left, right, down, up) for an eye of this aspect, outer side wider as a headset's
float4 xr_sim_eye_fov(byte eye, float aspect) {
    const float up = tanf(xr_sim_fov_up);
    const float side = up * aspect;
    const float shift = side * 0.1f * (eye == 0 ? 1 : -1);  // the left eye sees more to its left
    return (float4) { -atanf(side + shift), atanf(side - shift), -xr_sim_fov_up, xr_sim_fov_up };
}

// the head in the room: its neck where the script has it (walking, crouching), the head turned about it as a
//  person's (looking down, the eyes go forward and lower: fun/body.c xr_neck_room)
static inline float3 xr_sim_head_place(float4 head_rotation) {
    return float3_sub(float3_add(xr_sim_script_head_position, float4_rotate_float3(head_rotation, xr_neck_to_eyes)), xr_neck_to_eyes);
}

// the eye in the head's space: the head turned, the eye to its side
float4x4 xr_sim_eye_matrix(byte eye, float4 head_rotation) {
    // the head's place in the room (walking in it, scripted) then the eye's side of it, as a headset's eyes are
    const float3 offset = { (eye == 0 ? -0.5f : 0.5f) * xr_sim_eye_distance, 0, 0 };
    return float4x4_transform(float3_add(xr_sim_head_place(head_rotation), float4_rotate_float3(head_rotation, offset)), head_rotation);
}

static inline float xr_sim_axis(const PhysicalButton* negative, const PhysicalButton* positive) {
    return (float) positive->is_pressed - (float) negative->is_pressed;
}

static inline const Keyboard* xr_sim_keyboard(ecs* world) {
    return zox_valid(local_keyboard) ? zox_get(local_keyboard, Keyboard) : NULL;
}

static inline float4 xr_sim_head_rotation() {
    return quaternion_from_euler(xr_sim_head_euler);
}

// the frame: the head turned by its keys
static byte xr_sim_frame_begin(ecs* world, byte* running) {
    (void) running;
    const Keyboard* keyboard = xr_sim_keyboard(world);
    if (!keyboard) {
        return 0;
    }
    const float delta = (float) zox_delta_time;
    xr_sim_head_euler.y += xr_sim_axis(&keyboard->e, &keyboard->q) * xr_sim_head_speed * delta;
    xr_sim_head_euler.x += xr_sim_axis(&keyboard->f, &keyboard->r) * xr_sim_head_speed * delta;
    return 1;
}

// both eyes side by side in the window
static byte xr_sim_locate(ecs* world, float4* head_rotation, float3* head_position, xr_eye_view eyes[2]) {
    if (!zox_valid(main_app) || !zox_has(main_app, WindowSize)) {
        return 0;
    }
    *head_rotation = xr_sim_head_rotation();
    *head_position = xr_sim_head_place(*head_rotation);
    const int2 window = zox_getv(main_app, WindowSize);
    const float2 half = { window.x / 2, window.y };
    for (byte i = 0; i < 2; i++) {
        eyes[i] = (xr_eye_view) {
            .matrix = xr_sim_eye_matrix(i, *head_rotation),
            .fov = xr_sim_eye_fov(i, half.x / half.y),
            .viewport_position = { i * half.x, 0 },
            .viewport_size = half,
        };
    }
    return 1;
}

static byte xr_sim_inputs(ecs* world) {
    return xr_sim_keyboard(world) != NULL;
}

static float2 xr_sim_stick(ecs* world, byte hand) {
    const Keyboard* keyboard = xr_sim_keyboard(world);
    if (hand == xr_hand_left) {
        return float2_add(xr_sim_script_left_stick, (float2) { xr_sim_axis(&keyboard->a, &keyboard->d), xr_sim_axis(&keyboard->s, &keyboard->w) });
    }
    return float2_add(xr_sim_script_stick, (float2) { xr_sim_axis(&keyboard->left, &keyboard->right), xr_sim_axis(&keyboard->down, &keyboard->up) });
}

// whether the simulator holds a hand's button, from its key (the right hand's A, the left's menu)
static byte xr_sim_button(ecs* world, byte hand, byte type) {
    (void) hand;
    const Keyboard* keyboard = xr_sim_keyboard(world);
    switch (type) {
        case zox_btn_a: return xr_sim_script_a || keyboard->space.is_pressed || keyboard->enter.is_pressed;
        case zox_btn_b: return keyboard->backspace.is_pressed;
        case zox_btn_start: return xr_sim_script_start || keyboard->escape.is_pressed;
        default: return 0;
    }
}

// a hand's grip: held ahead of the head, to its side (or where a test puts it)
static byte xr_sim_hand(ecs* world, byte hand, float3* position, float4* rotation) {
    (void) world;
    const byte left = hand == xr_hand_left;
    if (xr_sim_script_hands_on) {
        *position = xr_sim_script_hands[left ? 0 : 1];
        *rotation = quaternion_identity;
        return 1;
    }
    const float4 head_rotation = xr_sim_head_rotation();
    const float3 offset = { left ? -xr_sim_hand_side : xr_sim_hand_side, -xr_sim_hand_drop, -xr_sim_hand_ahead };
    *position = float4_rotate_float3(head_rotation, offset);
    *rotation = head_rotation;
    return 1;
}

// where a hand points: its grip's pose, unless a test puts it apart (a headset's aim is ahead of the grip, tilted)
static byte xr_sim_aim(ecs* world, byte hand, float3* position, float4* rotation) {
    if (xr_sim_script_aims_on) {
        *position = xr_sim_script_aims[hand ? 1 : 0];
        *rotation = xr_sim_script_aim_rotations[hand ? 1 : 0];
        return 1;
    }
    return xr_sim_hand(world, hand, position, rotation);
}

static float xr_sim_delta(ecs* world) {
    (void) world;
    return (float) zox_delta_time;
}

static byte xr_sim_should_render(ecs* world) {
    (void) world;
    return rendering;
}

static byte xr_sim_eye_begin(ecs* world, byte eye) {
    (void) world;
    (void) eye;
    return 1;
}

static void xr_sim_eye_end(ecs* world, byte eye) {
    (void) world;
    (void) eye;
}

static void xr_sim_frame_end(ecs* world) {
    (void) world;
}

const xr_runtime xr_sim_runtime = {
    .name = "simulator",
    .frame_begin = xr_sim_frame_begin,
    .locate = xr_sim_locate,
    .inputs = xr_sim_inputs,
    .stick = xr_sim_stick,
    .button = xr_sim_button,
    .aim = xr_sim_aim,
    .grip = xr_sim_hand,
    .delta = xr_sim_delta,
    .should_render = xr_sim_should_render,
    .eye_begin = xr_sim_eye_begin,
    .eye_end = xr_sim_eye_end,
    .frame_end = xr_sim_frame_end,
    .flip_y = 0,
};

// XR plays by its controllers: the keyboard, mouse and touchscreen never switch the device mode (DeviceFeeder), on
//  the simulator they feed the controllers, on a headset they're stray (Android's pointer and touch events flipped
//  the mode off the gamepad, Disabled the controllers: in game the stick didn't walk, start didn't pause, while
//  the menus, reading the sticks direct, still worked)
void xr_feed_devices(ecs* world, entity app) {
    (void) app;
    if (!zox_xr_active) {
        return;
    }
    const entity devices[] = { local_keyboard, local_mouse, local_touchscreen };
    for (int i = 0; i < 3; i++) {
        if (zox_valid(devices[i])) {
            zox_add(devices[i], DeviceFeeder);
        }
    }
}
