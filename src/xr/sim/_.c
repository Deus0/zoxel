// The XR simulator: a headset on the desktop, both eyes side by side in the window (--xr-sim)
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

// whether the simulator holds a hand's button, from its key
static byte xr_sim_button(const Keyboard* keyboard, byte type) {
    switch (type) {
        case zox_btn_a: return xr_sim_script_a || keyboard->space.is_pressed || keyboard->enter.is_pressed;
        case zox_btn_b: return keyboard->backspace.is_pressed;
        case zox_btn_start: return xr_sim_script_start || keyboard->escape.is_pressed;
        default: return 0;
    }
}

// the keyboard into each XR controller's zevices and pose, as a headset's would be; the head turned
void xr_sim_controllers_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    if (!zox_xr_sim || !zox_valid(local_keyboard)) {
        return;
    }
    const Keyboard* keyboard = zox_get(local_keyboard, Keyboard);
    const float delta = (float) zox_delta_time;
    xr_sim_head_euler.y += xr_sim_axis(&keyboard->e, &keyboard->q) * xr_sim_head_speed * delta;
    xr_sim_head_euler.x += xr_sim_axis(&keyboard->f, &keyboard->r) * xr_sim_head_speed * delta;
    const float4 head_rotation = quaternion_from_euler(xr_sim_head_euler);
    xr_head_now_rotation = head_rotation;   // the head bone turns with it, the room walks it, this frame (fun/body.c)
    xr_head_now_position = xr_sim_head_place(head_rotation);
    zox_sys_begin();
    zox_sys_in(XrHand);
    zox_sys_out(DeviceHasInput);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(XrHand, hand);
        zox_sys_o(DeviceHasInput, has_input);
        const byte left = hand->value == xr_hand_left;
        const float2 stick = left ?
            float2_add(xr_sim_script_left_stick, (float2) { xr_sim_axis(&keyboard->a, &keyboard->d), xr_sim_axis(&keyboard->s, &keyboard->w) }) :
            float2_add(xr_sim_script_stick, (float2) { xr_sim_axis(&keyboard->left, &keyboard->right), xr_sim_axis(&keyboard->down, &keyboard->up) });
        byte any = stick.x != 0 || stick.y != 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                const entity e2 = it2.entities[j];
                if (zox_has(e2, ZeviceStick)) {
                    xr_controller_stick_set(world, e2, stick);
                } else if (zox_has(e2, ZeviceButton)) {
                    const byte held = xr_sim_button(keyboard, zox_getv(e2, DeviceButtonType));
                    xr_controller_button(world, e2, held);
                    any |= held;
                }
            }
        }
        has_input->value = any;
        if (xr_sim_script_hands_on) {
            xr_controller_pose(world, e, xr_sim_script_hands[left ? 0 : 1], quaternion_identity);
            continue;
        }
        const float3 offset = { left ? -xr_sim_hand_side : xr_sim_hand_side, -xr_sim_hand_drop, -xr_sim_hand_ahead };
        xr_controller_pose(world, e, float4_rotate_float3(head_rotation, offset), head_rotation);
    }
    zox_sys_on_end();
} zoxd_system(xr_sim_controllers_system);

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

void xr_sim_render_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    if (!zox_xr_sim || !rendering || !zox_valid(main_app) || !zox_has(main_app, WindowSize)) {
        return;
    }
    const float delta = (float) zox_delta_time;
    const byte roaming = xr_roam_input_enabled(world);
    const float2 move = roaming ? xr_controller_stick(world, xr_hand_left) : float2_zero;
    const float2 turn = roaming ? xr_controller_stick(world, xr_hand_right) : float2_zero;
    const float4 head_rotation = quaternion_from_euler(xr_sim_head_euler);
    xr_roam_update(world, xr_camera, move, turn, head_rotation, xr_sim_head_place(head_rotation), delta);
    const float4x4 body = xr_view_body;
    const int2 window = zox_getv(main_app, WindowSize);
    const float2 half = { window.x / 2, window.y };
    xr_cull_set(world, xr_sim_eye_fov(0, half.x / half.y), xr_sim_eye_fov(1, half.x / half.y));
    for (byte i = 0; i < 2; i++) {
        xr_render_eye(world, xr_sim_eye_matrix(i, head_rotation), xr_sim_eye_fov(i, half.x / half.y), body, (float2) { i * half.x, 0 }, half, 0);
    }
    zox_sys_on_end();
} zoxd_system(xr_sim_render_system);

void zox_systems_xr_sim(ecs* world) {
    zox_system_1(
        xr_sim_controllers_system,
        zoxp_inputs_extract,
        [in] xr.XrHand,
        [out] inputs.DeviceHasInput,
        [none] xr.XrController
    );
    zox_system_1(xr_sim_render_system, zoxp_render, 0);
}
