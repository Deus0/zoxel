float stick_cutoff = 0.06f;

byte get_gamepad_button(byte old_value, SDL_Joystick *joystick, int index, byte dbg_log) {
    byte value = SDL_JoystickGetButton(joystick, index);
    if (value && dbg_log) {
        zox_log("- SDL Jostick [%i]: %i", index, value);
    }
    return process_input_button(old_value, value);
}

float get_gamepad_axis_raw(SDL_Joystick *joystick, int index, byte dbg_log) {
    int raw_value = SDL_JoystickGetAxis(joystick, index);
    if (dbg_log) {
        zox_log("Raw SDL Axis [%i]:[%i]", index, raw_value);
    }
    raw_value = apply_joystick_deadzone(index, raw_value);
    float axis_value = raw_value / 32768.0f;
    if (float_abs(axis_value) <= joystick_min_cutoff) {
        axis_value = 0.0f;
    }
    if (axis_value < -1.0f || axis_value > 1.0f) {
        axis_value = 0;
    }
    axis_value = -axis_value;
    return axis_value; // invert as sdl inverts it first?
}

byte get_gamepad_axis(SDL_Joystick *joystick, ZeviceStick *stick,  int index_x, int index_y, byte dbg_log) {
    float2 previous_value = stick->value;
    stick->value.x = get_gamepad_axis_raw(joystick, index_x, dbg_log);
    stick->value.y = get_gamepad_axis_raw(joystick, index_y, dbg_log);
    if (float_abs(stick->value.x) > stick_cutoff || float_abs(stick->value.y) > stick_cutoff) {
        if (dbg_log) {
            zox_log(" > stick(%i) [%fx%f]", index_x, index_y, stick->value.x, stick->value.y);
        }
        return !(stick->value.x == previous_value.x && stick->value.y == previous_value.y);
    }
}

float get_gamepad_bumper(SDL_Joystick *joystick, int index, byte dbg_log) {
    int raw = SDL_JoystickGetAxis(joystick, index);
    float value = raw / 32768.0f;
    value += 1.0f;
    value /= 2.0f;
    if (dbg_log) {
        zox_log("Raw SDL Axis [%i]:[%i]:[%f]", index, raw, value);
    }
    return value;
}

byte get_gamepad_dpad(byte old_value, SDL_Joystick *joystick, int index) {
    byte is_pressed_down = 0;
    byte is_pressed_up = 0;
    byte is_pressed_left = 0;
    byte is_pressed_right = 0;
    Uint8 hatState = SDL_JoystickGetHat(joystick, 0);
    // Check the state of the D-pad
    switch (hatState) {
        case SDL_HAT_UP:
            is_pressed_up = 1;
            break;
        case SDL_HAT_DOWN:
            is_pressed_down = 1;
            break;
        case SDL_HAT_LEFT:
            is_pressed_left = 1;
            break;
        case SDL_HAT_RIGHT:
            is_pressed_right = 1;
            break;
        case SDL_HAT_LEFTUP:
            is_pressed_left = 1;
            is_pressed_up = 1;
            break;
        case SDL_HAT_RIGHTUP:
            is_pressed_right = 1;
            is_pressed_up = 1;
            break;
        case SDL_HAT_LEFTDOWN:
            is_pressed_left = 1;
            is_pressed_down = 1;
            break;
        case SDL_HAT_RIGHTDOWN:
            is_pressed_right = 1;
            is_pressed_down = 1;
            break;
        case SDL_HAT_CENTERED:
            // Handle no input
            break;
    }
    if (index == zox_device_button_dpad_down) {
        return process_input_button(old_value, is_pressed_down);
    } else if (index == zox_device_button_dpad_up) {
        return process_input_button(old_value, is_pressed_up);
    } else if (index == zox_device_button_dpad_left) {
        return process_input_button(old_value, is_pressed_left);
    } else if (index == zox_device_button_dpad_right) {
        return process_input_button(old_value, is_pressed_right);
    } else {
        return old_value;
    }
}

void extract_sdl_gamepad_button(ecs* world, entity e, SDL_Joystick* joystick, byte dbg_log) {
    if (!joystick) {
        return;
    }
    joystick_axes = SDL_JoystickNumAxes(joystick);
    byte rindex = zox_getv(e, RealButtonIndex);
    if (zox_has(e, ZeviceBumper)) {
        get_gamepad_bumper(joystick, rindex, dbg_log);
    } else if (zox_has(e, ZeviceStick)) {
        zox_mut_begin(e, ZeviceStick, stick);
        if (get_gamepad_axis(joystick, stick, rindex, rindex + 1, dbg_log)) {
            zox_modified(e, ZeviceStick);
        }
    } else if (zox_has(e, ZeviceButton)) {
        zox_mut_begin(e, ZeviceButton, button);
        byte new_value;
        if (is_dpad_button(rindex)) {
            new_value = get_gamepad_dpad(button->value, joystick, rindex);
        } else {
            new_value = get_gamepad_button(button->value, joystick, rindex, dbg_log);
        }
        if (new_value != button->value) {
            button->value = new_value;
            zox_mut_end(e, ZeviceButton);
        }
    }
}

byte sdl_gamepad_handle_disconnect(SDL_Joystick *joystick) {
    if (!joystick) {
        return 0;
    }
    if (!SDL_JoystickGetAttached(joystick)) {
        int joystick_id = SDL_JoystickInstanceID(joystick);
        const char* joystick_name = SDL_JoystickNameForIndex(joystick_id);
        zox_log("- Gamepad Disconnected [%i] [%s]", joystick_id, joystick_name);
        SDL_JoystickClose(joystick);
        joystick = NULL;
        return 0;
    }
    return 1;
}

zox_sys2(GamepadFetchSystem) {
    byte dbg_log = 0;
    byte dbg_log_all = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SDLGamepad);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SDLGamepad, gamepad);
        if (!sdl_gamepad_handle_disconnect(gamepad->value)) {
            if (dbg_log) {
                zox_log("SDL Joystick Disconnected [%s]", zox_get_name(e));
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
            extract_sdl_gamepad_button(world, e2, gamepad->value, dbg_log);
        }
        // Debug new gamepads!
        if (dbg_log_all) {
            // Axes
            int axis_count = SDL_JoystickNumAxes(gamepad->value);
            for (int j = 0; j < axis_count - 2; j++) {
                Sint16 value = SDL_JoystickGetAxis(gamepad->value, j);
                float value2 = value / 32767.0f;
                value2 = float_abs(value2) < 0.06f ? 0 : value2;
                if (value2) {
                    zox_log("Axis %i = %f", j, value2);
                }
            }
            // Buttons
            int button_count = SDL_JoystickNumButtons(gamepad->value);
            for (int j = 0; j < button_count; j++) {
                if (SDL_JoystickGetButton(gamepad->value, j)) {
                    zox_log("Button %i", j);
                }
            }
            // Hats (D-pad on many devices)
            int hat_count = SDL_JoystickNumHats(gamepad->value);
            for (int j = 0; j < hat_count; j++) {
                Uint8 hat = SDL_JoystickGetHat(gamepad->value, j);

                if (hat != SDL_HAT_CENTERED) {
                    zox_log("Hat %i = 0x%02X", j, hat);
                }
            }
        }
    }
} zox_sys_end(GamepadFetchSystem);
