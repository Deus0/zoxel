void handle_touch_down(ecs* world, entity finger, entity virtual_joystick, entity canvas) {
    if (!zox_valid(finger) || !zox_has(finger, ZevicePointer) || !zox_has(finger, ZevicePointerPosition)) {
        return;
    }
    byte button_type = zox_device_stick_left;
    byte finger_state = zox_getv(finger, ZevicePointer);
    int2 position = zox_getv(finger, ZevicePointerPosition);
    // zox_log("Finger [%s]", zox_get_name(finger));
    if (!devices_get_pressed_this_frame(finger_state)) {
        return;
    }
    /*entity device = zox_getv(finger, DeviceLink);
    if (!zox_valid(device)) { // || !zox_has(touchscreen, ScreenDimensions)) {
        zox_loge("Touchscreen invalid in [handle_touch_down]");
        return;
    }*/
    int2 size = zox_getv(canvas, LayoutSize);
    if (position.x >= size.x / 2) {
        button_type = zox_device_stick_right;
    }
    // zox_log("Spawning Virtual Joystick [%i]", button_type);
    spawn_virtual_joystick(world, canvas, position, finger, virtual_joystick, button_type);
}

// When finger is held down against the screen
void handle_touch_drag(ecs* world, entity finger, entity virtual_joystick) {
    if (!zox_valid(finger) || !zox_has(finger, ZevicePointer) || !zox_has(finger, ZevicePointerPosition)) {
        return;
    }
    zox_geter(finger, ZevicePointer, pointer);
    if (!devices_get_pressed(pointer->value)) {
        return;
    }
    if (!zox_has(virtual_joystick, ElementLink)) {
        return;
    }
    entity joystick_ui = zox_getv(virtual_joystick, ElementLink);
    if (!zox_valid(joystick_ui)) {
        return;
    }
    uint children_capacity = zox_children_capacity;
    entity children[children_capacity];
    uint children_length = zox_get_children(world, joystick_ui, children, children_capacity);
    if (!children_length) {
        // zox_logw("Finger has invalid childrens");
        return;
    }
    entity joystick_pointer = children[0];
    if (!zox_valid(joystick_pointer)) {
        zox_log_error("invalid joystick_pointer, parent [%s]", zox_get_name(joystick_ui))
        return;
    }
    zox_geter(finger, ZevicePointerPosition, finger_position);
    zox_geter(joystick_ui, LayoutPosition, virtual_joystick_position);
    zox_geter(joystick_ui, LayoutSize, joystick_size);
    zox_geter(joystick_pointer, LayoutSize, joystick_pointer_size);
    zox_muter(joystick_pointer, LayoutPosition, pointer_position);
    zox_muter(joystick_pointer, LayoutPositionDirty, pointer_position_dirty);
    int2 delta_position = int2_sub(finger_position->value, virtual_joystick_position->value);
    int2 size_limits = int2_multiply_float(int2_sub(joystick_size->value, joystick_pointer_size->value), 0.5f);
    // Set Pointer Position
    pointer_position->value.x = delta_position.x;
    pointer_position->value.y = delta_position.y;
    int2_limit(&pointer_position->value, size_limits);
    if (!pointer_position_dirty->value) {
        pointer_position_dirty->value = zox_dirty_trigger;
    }
    // Set Pointer Value
    zox_muter(virtual_joystick, ZeviceStick, stick);
    stick->value = (float2) {
        - pointer_position->value.x / (float) size_limits.x,
        pointer_position->value.y / (float) size_limits.y
    };
    /*zox_log("size_limits: %ix%i", size_limits.x, size_limits.y);
        *       zox_log("   pointer_position: %ix%i", pointer_position->value.x, pointer_position->value.x);
        *       zox_log("   virtual_joystick_position: %ix%i", virtual_joystick_position->value.x, virtual_joystick_position->value.x);*/
}

// Destroy the UI when released
void handle_touch_release(ecs* world, entity finger, entity zevice) {
    if (!zox_valid(finger) || !zox_has(finger, ZevicePointer)) {
        return;
    }
    zox_geter(finger, ZevicePointer, pointer);
    if (!devices_get_released_this_frame(pointer->value)) {
        return;
    }
    if (!zox_has(zevice, ElementLink)) {
        return;
    }
    entity joystick_ui = zox_getv(zevice, ElementLink);
    // zox_log("Finger Released - UI: [%lu]", joystick_ui);
    if (!zox_valid(joystick_ui)) {
        return;
    }
    zox_delete(joystick_ui)  // assume not touching
    zox_set(zevice, ElementLink, { 0 });
    // Reset on release
    zox_muter(zevice, ZeviceStick, stick);
    stick->value = float2_zero;
}

// NOTE: This runs from a Zevice
zox_sys2(VirtualJoystickSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLink);
    zox_sys_in(RaycasterResult);
    zox_sys_in(ZevicePointer);
    zox_sys_in(VirtualZeviceLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RaycasterResult, result);
        zox_sys_i(DeviceLink, device);
        zox_sys_i(ZevicePointer, pointer);
        zox_sys_i(VirtualZeviceLink, vzevice);
        if (!zox_dbg_touch_with_mouse && !zox_has(e, Finger)) {
            continue;
        }
        if (!zox_valid(device->value)) {
            continue;
        }
        entity player = zox_get_parent(world, device->value);
        if (!zox_valid(player)) {
            continue;
        }
        entity game = zox_getv(player, GameLink);
        entity canvas = zox_getv(player, CanvasLink);
        if (!zox_valid(game) || !zox_valid(canvas)) {
            return;
        }
        byte click_value = pointer->value;
        // NOTE: Case for if we Clicked the UI element
        if (devices_get_pressed_this_frame(click_value) && result->value) {
            continue;
        }
        byte game_state = zox_getv(game, GameState);
        if (game_state == zox_game_playing) {
            handle_touch_down(world, e, vzevice->value, canvas);
        }
        handle_touch_drag(world, e, vzevice->value);
        handle_touch_release(world, e, vzevice->value);
    }
} zox_sys_end(VirtualJoystickSystem);
