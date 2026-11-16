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

// the hand in the world: its tracked pose (in the head's tracking space) on top of the roam's body
void xr_controller_pose(ecs* world, entity device, float3 position, float4 rotation) {
    const float4 body = free_roam_rotation(&xr_roam);
    zox_setv(device, Position3D, float3_add(xr_roam.position, float4_rotate_float3(body, position)));
    zox_setv(device, Rotation3D, quaternion_rotate(body, rotation));
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
