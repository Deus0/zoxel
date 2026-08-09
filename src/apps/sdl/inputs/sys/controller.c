float stick_cutoff = 0.06f;

byte get_controller_button(byte old_value, zox_sdl_gamepad* controller, zox_sdl_gamepad_button btn, byte dbg_log) {
    byte value = sdl_get_gamepad_button(controller, btn);
    /*if (value && dbg_log) {
        zox_log("SDL GameController Button [%i]: %i", btn, value);
    }*/
    return process_input_button(old_value, value);
}

float get_gamepad_axis_raw(zox_sdl_gamepad* controller, zox_sdl_gamepad_axis axis, byte dbg_log) {
    Sint16 raw_value = sdl_get_gamepad_axis(controller, axis);
    raw_value = apply_joystick_deadzone(axis, raw_value);
    float value = raw_value / 32768.0f;
    if (float_abs(value) <= joystick_min_cutoff) {
        value = 0.0f;
    }
    if (value < -1.0f || value > 1.0f) {
        value = 0;
    }
    if (dbg_log && value) {
        zox_log("SDL Axis [%i]:[%i]", axis, value);
    }
    return value;
}

byte get_gamepad_axis(zox_sdl_gamepad* controller, ZeviceStick *stick, zox_sdl_gamepad_axis axis_x, zox_sdl_gamepad_axis axis_y, byte dbg_log) {
    float2 previous_value = stick->value;
    stick->value.x = -get_gamepad_axis_raw(controller, axis_x, dbg_log);
    stick->value.y = -get_gamepad_axis_raw(controller, axis_y, dbg_log);
    if (float_abs(stick->value.x) > stick_cutoff ||
        float_abs(stick->value.y) > stick_cutoff) {
        if (dbg_log) {
            zox_log(" > stick [%fx%f]", stick->value.x, stick->value.y);
        }
        return !(stick->value.x == previous_value.x &&
                 stick->value.y == previous_value.y);
    }
    return 0;
}

float get_gamepad_trigger(zox_sdl_gamepad* controller, zox_sdl_gamepad_axis axis, byte dbg_log) {
    Sint16 raw = sdl_get_gamepad_axis(controller, axis);
    float value = (raw + 32768.0f) / 65535.0f;
    if (float_abs(value) <= joystick_min_cutoff) {
        value = 0.0f;
    }
    if (dbg_log && value) {
        // zox_log("Trigger Axis [%i]:[%f]", axis, value);
    }
    return value;
}

void sdl_controller_extract_button(ecs* world, entity e, zox_sdl_gamepad* controller, byte dbg_log) {
    byte rindex = zox_getv(e, RealButtonIndex);
    if (zox_has(e, ZeviceBumper)) {
        zox_sdl_gamepad_axis trigger;
        if (rindex == zox_stk_left) {
            trigger = ZOX_GAMEPAD_AXIS_LEFT_TRIGGER;
        } else {
            trigger = ZOX_GAMEPAD_AXIS_RIGHT_TRIGGER;
        }
        float value = get_gamepad_trigger(controller, trigger, dbg_log);
        byte button_value = value >= 0.9f;
        zox_mut_begin(e, ZeviceButton, button);
        byte new_value = process_input_button(button->value, button_value);
        if (button->value != new_value) {
            button->value = new_value;
            if (dbg_log) {
                zox_log("SDL Bumper Button [%i]: %f  %i", rindex, value, new_value);
            }
            zox_mut_end(e, ZeviceButton);
        }
    } else if (zox_has(e, ZeviceStick)) {
        zox_mut_begin(e, ZeviceStick, stick);
        zox_sdl_gamepad_axis x, y;
        if (rindex == zox_stk_left) {
            x = ZOX_GAMEPAD_AXIS_LEFTX;
            y = ZOX_GAMEPAD_AXIS_LEFTY;
        } else {
            x = ZOX_GAMEPAD_AXIS_RIGHTX;
            y = ZOX_GAMEPAD_AXIS_RIGHTY;
        }
        byte changed = get_gamepad_axis(controller, stick, x, y, dbg_log);
        if (changed) {
            zox_modified(e, ZeviceStick);
        }
    } else if (zox_has(e, ZeviceButton)) {
        zox_mut_begin(e, ZeviceButton, button);
        byte value = get_controller_button(button->value, controller, (zox_sdl_gamepad_button) rindex, dbg_log);
        if (value != button->value) {
            button->value = value;
            if (value && dbg_log) {
                zox_log("SDL GameController Button [%i]: %i", rindex, value);
            }
            zox_mut_end(e, ZeviceButton);
        }
    }
}

zox_sys2(SdlControllerFetchSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(SdlGameController);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(SdlGameController, controller);
        if (!zox_sdl_gamepad_connected(controller->value)) {
            zox_sdl_gamepad_close(controller->value);
            if (dbg_log) {
                zox_log("SdlGameController Disconnected [%s]", zox_get_name(e));
            }
            entity player = zox_get_parent(world, e);
            if (zox_valid(player)) {
                zox_set(player, DeviceMode, { zox_device_mode_none });
            } else {
                zox_loge("No player parent of gamepad..");
            }
            zox_delete(e);
            continue;
        }
        // Fetch
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(world, e, children, children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            sdl_controller_extract_button(world, e2, controller->value, dbg_log);
        }
    }
} zox_sys_end(SdlControllerFetchSystem);
