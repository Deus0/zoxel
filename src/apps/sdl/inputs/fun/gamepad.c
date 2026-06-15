int joysticks_count;
int joystick_axes;
int last_axis_index;
int last_clicked_index = 0;

#ifdef zox_sdl

void initialize_sdl_gamepads(ecs *world, entity app) {
    // SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    joysticks_count = SDL_NumJoysticks();
    if (joysticks_count == 0) {
        return;
    }
    zox_logv("Gamepads Connected [%d]", joysticks_count);
    for (int i = 0; i < joysticks_count; i++) {
        if (using_sdl_gamecontrollers) {
            if (SDL_IsGameController(i)) {
                SDL_GameController* controller = SDL_GameControllerOpen(i);
                if (!controller) {
                    fprintf(stderr, "Joystick Error: %s\n", SDL_GetError());
                    continue;
                }
                zox_log("Controller Was Connected: %s", SDL_GameControllerName(controller));
                spawn_gamepad_sdl_controller(world, app, controller);
            }
        } else {
            SDL_Joystick *joystick = SDL_JoystickOpen(i);
            if (!joystick) {
                fprintf(stderr, "Joystick Error: %s\n", SDL_GetError());
                continue;
            }
            spawn_gamepad_sdl_joystick(world, app, joystick);
        }
    }
}

byte process_input_button(byte old_byte, byte raw_value) {
    byte was_pressed = devices_get_pressed(old_byte);
    byte pressed_this_frame = !was_pressed && raw_value;
    byte released_this_frame = was_pressed && !raw_value;
    /*if (pressed_this_frame) {
        zox_logv("  [%i] is pressed this frame", index);
    }
    if (released_this_frame) {
        zox_logv("  [%i] is released this frame", index);
    }*/
    byte new_value = 0;
    if (pressed_this_frame) {
        devices_set_pressed_this_frame(&new_value, 1);
    }
    if (released_this_frame) {
        devices_set_released_this_frame(&new_value, 1);
    }
    if (raw_value) {
        devices_set_is_pressed(&new_value, raw_value);
    }
    /*if (new_value != old_byte) {
        zox_logv("   - [%i] has updated [%i > %i]", index, old_value, new_value);
    }*/
    return new_value;
}


void debug_button(const PhysicalButton *button, const char *button_name) {
    if (button->pressed_this_frame) {
        zox_log("[%s] button pushed", button_name);
    } else if (button->released_this_frame) {
        zox_log("[%s] button released", button_name);
    }
}

void debug_stick(const PhysicalStick *physical_stick, const char *button_name) {
    if (float_abs(physical_stick->value.x) > joystick_cutoff_buffer && float_abs(physical_stick->value.y) > joystick_cutoff_buffer) {
        zox_log("[%s] stick pushed [%fx%f]", button_name, physical_stick->value.x, physical_stick->value.y);
    }
}
#else

void initialize_sdl_gamepads(ecs *world, entity app) { }

#endif
