// #define zoxel_mouse_emulate_touch

int virtual_joysticks_spawn_count = 0;
byte first_joystick_type = zox_device_stick_left;

void handle_touch_drag(ecs *world, entity canvas, entity finger, entity virtual_joystick, byte is_game_state_playing) {
    if (!zox_valid(finger) || !zox_has(finger, ZevicePointer) || !zox_has(finger, ZevicePointerPosition)) {
        return;
    }
    zox_geter(finger, ZevicePointer, pointer);
    if (devices_get_pressed_this_frame(pointer->value)) {
        zox_geter(finger, ZevicePointerPosition, position);
        if (is_game_state_playing) {
            byte button_type = zox_device_stick_left;
            zox_geter_value(finger, DeviceLink, entity, touchscreen);
            if (!zox_valid(touchscreen) || !zox_has(touchscreen, ScreenDimensions)) {
                zox_log_error("touchscreen invalid");
                return;
            }
            zox_geter_value(canvas, LayoutSize, int2, size);
            // zox_geter_value(touchscreen, ScreenDimensions, int2, size);
            if (position->value.x >= size.x / 2) {
                button_type = zox_device_stick_right;
            }
            spawn_virtual_joystick(world, canvas, position->value, finger, virtual_joystick, button_type);
            virtual_joysticks_spawn_count++;
        }
    } else {
        // ui wasn't spawned, return
        if (!zox_has(virtual_joystick, ElementLink)) {
            return;
        }
        zox_geter_value(virtual_joystick, ElementLink, entity, joystick);
        if (!zox_valid(joystick)) {
            return;
        }
        // When Finger Released
        if (devices_get_released_this_frame(pointer->value)) {
            zox_muter(virtual_joystick, ZeviceStick, zeviceStick);
            zeviceStick->value = float2_zero;
            if (joystick) {
                zox_delete(joystick);
                zox_set(virtual_joystick, ElementLink, { 0 });
                virtual_joysticks_spawn_count--;
            }
        }
        // When Finger is Dragging
        else if (devices_get_pressed(pointer->value)) {
            uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(world, joystick, children, children_capacity);
            if (!children_length) {
                zox_loge("Finger has invalid childrens");
                return;
            }
            entity joystick_pointer = children[0];
            if (!zox_valid(joystick_pointer)) {
                zox_log_error("invalid joystick_pointer, parent [%s]", zox_get_name(joystick))
                return;
            }
            zox_geter(finger, ZevicePointerPosition, finger_position);
            zox_geter(joystick, LayoutPosition, virtual_joystick_position);
            zox_geter(joystick, LayoutSize, joystick_size);
            zox_geter(joystick_pointer, LayoutSize, joystick_pointer_size);
            zox_muter(joystick_pointer, LayoutPosition, pointer_position);
            zox_muter(joystick_pointer, LayoutPositionDirty, pointer_position_dirty);
            zox_muter(virtual_joystick, ZeviceStick, stick);
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
            float2 input_value = (float2) {
                pointer_position->value.x / (float) size_limits.x,
                pointer_position->value.y / (float) size_limits.y
            };
            stick->value = input_value;
            /*zox_log("size_limits: %ix%i", size_limits.x, size_limits.y);
            zox_log("   pointer_position: %ix%i", pointer_position->value.x, pointer_position->value.x);
            zox_log("   virtual_joystick_position: %ix%i", virtual_joystick_position->value.x, virtual_joystick_position->value.x);*/
        } else {
            if (joystick) {
                zox_delete(joystick)  // assume not touching
                zox_set(virtual_joystick, ElementLink, { 0 })
                virtual_joysticks_spawn_count--;
            }
        }
    }
}

zox_sys2(VirtualJoystickSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLink);
    zox_sys_in(RaycasterResult);
    zox_sys_in(ZevicePointer);
    zox_sys_in(VirtualZeviceLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RaycasterResult, raycasterResult);
        zox_sys_i(DeviceLink, deviceLink);
        zox_sys_i(ZevicePointer, pointer);
        zox_sys_i(VirtualZeviceLink, virtualZeviceLink);
        if (!deviceLink->value || !zox_has(e, Finger)) {
            continue;
        }
        // if (raycasterResult->value) continue;   // if raycasted ui, don't process
        // todo: use a DeviceMode for logic flow
        entity player = zox_get_value(deviceLink->value, PlayerLink)
        if (!zox_valid(player)) {
            continue;
        }
        // const byte device_mode = zox_get_value(player, DeviceMode)
        entity game = zox_get_value(player, GameLink)
        if (!zox_valid(game)) {
            return;
        }
        zox_geter_value(game, GameState, entity, game_state);
        zox_geter_value(player, CanvasLink, entity, canvas);
        byte is_game_state_playing = game_state == zox_game_playing;
        byte click_value = pointer->value;
        // if going to spawn a new one, continue
        if (devices_get_pressed_this_frame(click_value) && raycasterResult->value) {
            continue;
        }
        entity virtual_joystick = virtualZeviceLink->value;
        handle_touch_drag(world, canvas, e, virtual_joystick, is_game_state_playing);
    }
} zox_sys_end(VirtualJoystickSystem);
