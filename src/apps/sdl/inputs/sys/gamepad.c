byte get_gamepad_button(byte old_value, SDL_Joystick *joystick, int index, byte dbg_log) {
    byte value = SDL_JoystickGetButton(joystick, index);
    if (value && dbg_log) {
        zox_log("- SDL Jostick [%i]: %i", index, value);
    }
    return process_byte(old_value, value);
}

void extract_sdl_gamepad_button(ecs* world, entity e, SDL_Joystick* joystick, byte dbg_log) {
    if (!joystick) {
        return;
    }
    joystick_axes = SDL_JoystickNumAxes(joystick);
    byte rindex = zox_getv(e, RealButtonIndex);
    if (zox_has(e, ZeviceStick)) {
        ZeviceStick *stick = zox_get_mut(e, ZeviceStick);
        if (set_gamepad_axis2(stick, joystick, rindex)) {
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
    if (joystick == NULL) {
        return 0;
    }
    if (!SDL_JoystickGetAttached(joystick)) {
        int joystick_id = SDL_JoystickInstanceID(joystick);
        fprintf(stderr, "   > gamepad [%d] has disconnected\n", joystick_id);
        SDL_JoystickClose(joystick);
        joystick = NULL;
        return 0;
    }
    return 1;
}

zox_sys2(GamepadExtractSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SDLGamepad);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SDLGamepad, gamepad);
        if (!sdl_gamepad_handle_disconnect(gamepad->value)) {
            continue;
        }
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(world, e, children, children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            extract_sdl_gamepad_button(world, e2, gamepad->value, dbg_log);
        }
    }
} zox_sys_end(GamepadExtractSystem);
