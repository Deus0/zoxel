byte touchscreen_is_any_input(ecs *world, entity e) {
    if (!e || !zox_valid(e)) {
        return 0;
    }
    uint children_capacity = zox_children_capacity;
    entity children[children_capacity];
    uint children_length = zox_get_children(world, e, children, children_capacity);
    for (uint i = 0; i < children_length; i++) {
        entity e2 = children[i];
        if (!e2 || !zox_has(e2, ZevicePointer) || !zox_has(e2, Finger)) {
            continue;
        }
        const ZevicePointer* zevicePointer = zox_get(e2, ZevicePointer);
        if (zevice_pointer_has_input(zevicePointer)) {
            return 1;
        }
    }
    return 0;
}

byte mouse_is_any_input(ecs *world, entity e) {
    if (!e || !zox_valid(e)) {
        return 0;
    }
    uint children_capacity = zox_children_capacity;
    entity children[children_capacity];
    uint children_length = zox_get_children(world, e, children, children_capacity);
    for (uint i = 0; i < children_length; i++) {
        entity e2 = children[i];
        if (!zox_has(e2, ZevicePointer)) {
            continue;
        }
        const ZevicePointer* zevicePointer = zox_get(e2, ZevicePointer);
        if (zevice_pointer_has_input(zevicePointer)) {
            return 1;
        }
    }
    return 0;
}

// a device's button of a type (zox_btn_*) is down
static inline byte device_button_down(ecs* world, entity device, byte type) {
    byte pressed = 0;
    iter it2 = zox_children(world, device);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count && !pressed; j++) {
            const entity e = it2.entities[j];
            pressed = zox_has(e, ZeviceButton) && zox_has(e, DeviceButtonType) &&
                zox_getv(e, DeviceButtonType) == type &&
                (devices_get_pressed(zox_getv(e, ZeviceButton)) ||
                devices_get_pressed_this_frame(zox_getv(e, ZeviceButton)));
        }
    }
    return pressed;
}

// a gamepad unplugged: its player has no device until another gamepad reaches it (reclaim_gamepads, huds) or it
//  switches to another of its own, the gamepad goes
void on_gamepad_disconnected(ecs* world, entity gamepad) {
    const entity player = zox_get_parent(world, gamepad);
    if (zox_valid(player) && zox_has(player, DeviceMode)) {
        zox_setv(player, DeviceMode, zox_device_mode_none);
    }
    zox_delete(gamepad);
}
