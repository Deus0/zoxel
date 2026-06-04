int joysticks_count;
int joystick_axes;
int last_axis_index;
int last_clicked_index = 0;

#ifdef zox_sdl

extern entity dbg_player;

entity spawn_sdl_gamepad(ecs* world, SDL_Joystick* joystick) {
    byte gamepad_type = get_gamepad_type(joystick);
    entity e = spawn_gamepad(world, gamepad_type);
    zox_set(e, SDLGamepad, { joystick });
    zox_log_input("   + gamepad [%s]", SDL_JoystickName(joystick));
    if (zox_valid(dbg_player)) {
        zox_set_parent(world, e, dbg_player);
    } else {
        zox_loge("No dbg_player in [spawn_sdl_gamepad]");
    }
    return e;
}

void handle_new_sdl_gamepad(ecs *world, SDL_Event event) {
    SDL_Joystick* joystick = SDL_JoystickOpen(event.jdevice.which);
    if (!joystick) {
        fprintf(stderr, "   ! joystick error: %s\n", SDL_GetError());
        return;
    }
    zox_log("+ New Gamepad [%d]", SDL_JoystickInstanceID(joystick));
    spawn_sdl_gamepad(world, joystick);
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
            spawn_sdl_gamepad(world, joystick);
        }
    }
}

byte process_input_button(byte old_byte, byte raw_value) {
    byte was_pressed = devices_get_pressed(old_byte);
    byte pressed_this_frame = !was_pressed && raw_value;
    byte released_this_frame = was_pressed && !raw_value;
    if (pressed_this_frame) {
        zox_log_input("  [%i] is pressed this frame", index);
    }
    if (released_this_frame) {
        zox_log_input("  [%i] is released this frame", index);
    }
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
    if (new_value != old_byte) {
        zox_log_input("   - [%i] has updated [%i > %i]", index, old_value, new_value);
    }
    return new_value;
}


void debug_button(const PhysicalButton *button, const char *button_name) {
    if (button->pressed_this_frame) {
        zox_log(" > [%s] button pushed\n", button_name);
    } else if (button->released_this_frame) {
        zox_log(" > [%s] button released\n", button_name);
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
