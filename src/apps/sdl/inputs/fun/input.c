void close_sdl_input() {
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

void initialize_sdl_input() {
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    if (!sdl_init_subsystem(SDL_INIT_JOYSTICK)) {
        fprintf(stderr, "Failed SDL joystick subsystem: %s\n", SDL_GetError());
    }
}

byte update_sdl_input(ecs *world, entity app, SDL_Event event) {
    // sdl_extract_keyboard(world, event);
    sdl_extract_mouse_wheel(event);
    // Handle Added Controllers
    if (using_sdl_gamecontrollers) {
        if (event.type == ZOX_CONTROLLERDEVICEADDED) {
            int device_index = event.cdevice.which;
            zox_sdl_gamepad* controller = zox_sdl_gamepad_open(device_index);
            zox_log("Controller Connected [%i]:[%s]", device_index, zox_sdl_gamepad_name(controller));
            spawn_gamepad_sdl_controller(world, app, controller);
            return 1;
        } else if (event.type == ZOX_CONTROLLERDEVICEREMOVED) {
            SDL_JoystickID id = event.cdevice.which;
            zox_log("Controller Disconnected [%i]", id);
            return 1;
        } else if (event.type == ZOX_CONTROLLERDEVICEREMAPPED) {
            zox_log("Controller Remapped");
            return 1;
        }
    } /*else {
        if (event.type == SDL_JOYDEVICEADDED) {
            int device_index = event.jdevice.which;
            zox_log("Joystick Connected [%i]", device_index);
            // zox_log("Joystick Connected [%i]:[%s]", device_index,  SDL_JoystickNameForIndex(device_index));
            SDL_Joystick* joystick = SDL_JoystickOpen(event.jdevice.which);
            if (!joystick) {
                fprintf(stderr, "Joystick Error: %s\n", SDL_GetError());
                return 0;
            }
            zox_log("New Joystick [%d]", SDL_JoystickInstanceID(joystick));
            spawn_gamepad_sdl_joystick(world, app, joystick);
            return 1;
        } else if (event.type == SDL_JOYDEVICEREMOVED) {
            int device_index = event.jdevice.which;
            zox_log("Joystick Disconnected [%i]:[%s]", device_index,  SDL_JoystickNameForIndex(device_index));
            return 1;
        }
    }*/
    return 0;
}
