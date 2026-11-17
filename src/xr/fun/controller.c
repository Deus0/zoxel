// XR controllers as input devices, one a hand, each a device (inputs) with its pose, so the ui navigates and
//  clicks, and the device mode switches, as a gamepad's do, without the rest of the engine knowing about XR
//  (doc/plans/xr.md step 3)
//  - the state is the device's: its zevices (a stick, buttons) and its pose (Position3D, Rotation3D, the hand in
//      the world: the roam's body, then the hand), written each frame by a headset (openxr/controller.c) or the
//      simulator (sim/)
//  - Gamepad: the device mode they play as; DeviceNavigator: the right hand's, it navigates (its stick) and clicks
//  - the right hand: stick, A, B, grip (RB), trigger (RT); the left: stick, X, Y, grip (LB), trigger (LT), menu
//  - the right trigger clicks as A does (point and pull, as on a headset), its source writes both
//  - sticks as SDL's gamepads have them (x flipped from OpenXR's), xr_controller_stick flips them back

// a held button's next zevice state: pressed or released this frame on a change
static inline byte xr_button_next(byte old, byte pressed) {
    const byte was_pressed = devices_get_pressed(old);
    byte next = 0;
    if (!was_pressed && pressed) {
        devices_set_pressed_this_frame(&next, 1);
    }
    if (was_pressed && !pressed) {
        devices_set_released_this_frame(&next, 1);
    }
    if (pressed) {
        devices_set_is_pressed(&next, 1);
    }
    return next;
}

// sets a held button on its zevice, pressed or released this frame on a change
void xr_controller_button(ecs* world, entity zevice, byte held) {
    const byte old = zox_getv(zevice, ZeviceButton);
    const byte next = xr_button_next(old, held);
    if (next != old) {
        zox_setv(zevice, ZeviceButton, next);
    }
}

// sets a stick, as OpenXR has it (x right, y up)
void xr_controller_stick_set(ecs* world, entity zevice, float2 stick) {
    zox_setv(zevice, ZeviceStick, ((float2) { -stick.x, stick.y }));
}

// a hand's stick, as OpenXR has it, read back from its zevice (free roam)
float2 xr_controller_stick(ecs* world, byte hand) {
    const entity device = xr_controllers[hand];
    if (!zox_valid(device)) {
        return float2_zero;
    }
    iter it2 = zox_children(world, device);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            if (zox_has(it2.entities[j], ZeviceStick)) {
                const float2 stick = zox_getv(it2.entities[j], ZeviceStick);
                return (float2) { -stick.x, stick.y };
            }
        }
    }
    return float2_zero;
}

// each hand's pose as tracked, in the room (the tracking space), kept for the hands' ik (fun/hands.c): it puts them
//  on the body after physics has moved it, where the controller entities' poses were set before it moved
float3 xr_hand_tracked_position[2];
float4 xr_hand_tracked_rotation[2] = { { 0, 0, 0, 1 }, { 0, 0, 0, 1 } };
byte xr_hand_tracked[2];

// the hand in the world: its tracked pose on top of the body (the character in a game, fun/body.c, else the roam)
void xr_controller_pose(ecs* world, entity device, float3 position, float4 rotation) {
    const byte hand = zox_getv(device, XrHand) ? 1 : 0;
    xr_hand_tracked_position[hand] = position;
    xr_hand_tracked_rotation[hand] = rotation;
    xr_hand_tracked[hand] = 1;
    xr_body_frame frame;
    if (!xr_body_frame_get(world, xr_camera, xr_head_now_rotation, xr_head_now_position, &frame)) {
        frame.position = xr_roam.position;
        frame.rotation = free_roam_rotation(&xr_roam);
    }
    zox_setv(device, Position3D, xr_body_frame_point(frame, position));
    zox_setv(device, Rotation3D, quaternion_rotate(frame.rotation, rotation));
}

// sticks drive the menus while one's open, so the head doesn't fly off from them
byte xr_roam_input_enabled(ecs* world) {
    const entity player = zox_players[0];
    if (!zox_valid(player)) {
        return 1;
    }
    const entity game = zox_get_link(world, player, GameLink);
    return zox_valid(game) && zox_getv(game, GameState) == zox_game_state_playing;
}
