int joysticks_count;
// debug purposes
int joystick_axes;
int last_axis_index;
int last_clicked_index = 0;

#ifdef zox_sdl

entity spawn_gamepad_from_sdl(ecs *world, SDL_Joystick *joystick) {
    byte gamepad_type = get_gamepad_type(joystick);
    entity e = spawn_gamepad(world, gamepad_type);
    zox_set(e, SDLGamepad, { joystick })
    zox_log_input("   + gamepad [%s]", SDL_JoystickName(joystick))
    return e;
}

void handle_new_sdl_gamepad(ecs *world, SDL_Event event) {
    SDL_Joystick *joystick = SDL_JoystickOpen(event.jdevice.which);
    if (!joystick) {
        fprintf(stderr, "   ! joystick error: %s\n", SDL_GetError());
        return;
    }
    zox_log_input(" + new gamepad [%d] has connected", SDL_JoystickInstanceID(joystick))
    spawn_gamepad_from_sdl(world, joystick);
}

void initialize_sdl_gamepads(ecs *world, entity app) {
    // SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    joysticks_count = SDL_NumJoysticks();
    if (joysticks_count == 0) {
        return;
    }
    zox_log_input(" > gamepads connected [%d]", joysticks_count)
    for (int i = 0; i < joysticks_count; i++) {
        SDL_Joystick *joystick = SDL_JoystickOpen(i);
        if (!joystick) {
            fprintf(stderr, "   ! joystick error: %s\n", SDL_GetError());
        } else {
            spawn_gamepad_from_sdl(world, joystick);
        }
    }
}

byte process_byte(byte old_byte, byte raw_value) {
    byte was_pressed = devices_get_pressed(old_byte);
    byte pressed_this_frame = !was_pressed && raw_value;
    byte released_this_frame = was_pressed && !raw_value;
    if (pressed_this_frame) zox_log_input("  [%i] is pressed this frame", index)
    if (released_this_frame) zox_log_input("  [%i] is released this frame", index)
    byte new_value = 0;
    if (pressed_this_frame) devices_set_pressed_this_frame(&new_value, 1);
    if (released_this_frame) devices_set_released_this_frame(&new_value, 1);
    if (raw_value) devices_set_is_pressed(&new_value, raw_value);
    if (new_value != old_byte) zox_log_input("   - [%i] has updated [%i > %i]", index, old_value, new_value)
    return new_value;
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
    if (index == zox_device_button_dpad_down) return process_byte(old_value, is_pressed_down);
    else if (index == zox_device_button_dpad_up) return process_byte(old_value, is_pressed_up);
    else if (index == zox_device_button_dpad_left) return process_byte(old_value, is_pressed_left);
    else if (index == zox_device_button_dpad_right) return process_byte(old_value, is_pressed_right);
    return old_value;
}


void debug_button(const PhysicalButton *button, const char *button_name) {
    if (button->pressed_this_frame) {
        zox_log(" > [%s] button pushed\n", button_name)
    } else if (button->released_this_frame) {
        zox_log(" > [%s] button released\n", button_name)
    }
}

void debug_stick(const PhysicalStick *physical_stick, const char *button_name) {
    if (float_abs(physical_stick->value.x) > joystick_cutoff_buffer && float_abs(physical_stick->value.y) > joystick_cutoff_buffer) {
        zox_log(" > [%s] stick pushed [%fx%f]\n", button_name, physical_stick->value.x, physical_stick->value.y)
    }
}
#else

void initialize_sdl_gamepads(ecs *world, entity app) { }

void handle_new_sdl_gamepad(ecs *world, const SDL_Event event) { }

#endif
