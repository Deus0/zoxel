// XR controllers, a device a hand (fun/controller.c)
entity xr_controllers[2];
entity prefab_xr_controller;

entity spawn_prefab_xr_controller(ecs* world) {
    zox_prefab_child(prefab_device);
    zox_prefab_name("xr_controller");
    zox_add(e, XrController);
    zox_add(e, Gamepad);
    zox_setv(e, XrHand, 0);
    zox_setv(e, EntityTarget, 0);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    return e;
}

// a hand's buttons, as gamepad buttons: the right A B (and RB RT), the left X Y start (and LB LT)
entity spawn_xr_controller(ecs* world, entity player, byte hand) {
    zox_instance(prefab_xr_controller);
    zox_name(hand == xr_hand_left ? "xr_controller_left" : "xr_controller_right");
    zox_set_parent(world, e, player);
    zox_setv(e, XrHand, hand);
    const byte right = hand == xr_hand_right;
    const byte buttons[] = {
        right ? zox_btn_a : zox_btn_x,
        right ? zox_btn_b : zox_btn_y,
        right ? zox_btn_rb : zox_btn_lb,
        right ? zox_btn_rt : zox_btn_lt,
        zox_btn_start,
    };
    const byte count = right ? 4 : 5;
    for (byte i = 0; i < count; i++) {
        spawn_device_button(world, prefab_zevice_button, e, buttons[i], i);
    }
    spawn_zevice_stick(world, e, hand, hand);
    // the right hand navigates the ui (its stick) and clicks (A, its trigger), one selection between the hands
    if (right) {
        prefab_add_navigator(world, e);
    }
    return e;
}

void xr_spawn_controllers(ecs* world, entity player) {
    for (byte i = 0; i < 2; i++) {
        xr_controllers[i] = spawn_xr_controller(world, player, i);
    }
}

