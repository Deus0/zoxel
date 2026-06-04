void close_sdl_input() {
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

void initialize_sdl_input() {
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) fprintf(stderr, "Failed SDL joystick subsystem: %s\n", SDL_GetError());
}

void input_reset_sdl() {
    sdl_reset_mouse_wheel();
}

byte update_sdl_input(ecs *world, SDL_Event event) {
    sdl_extract_keyboard(world, event);
    sdl_extract_mouse_wheel(event);
    if (event.type == SDL_JOYDEVICEADDED) {
        int device_index = event.jdevice.which;
        zox_log("+ Joystick Connected [%i]:[%s]", device_index,  SDL_JoystickNameForIndex(device_index));
        handle_new_sdl_gamepad(world, event);
        return 1;
    } else if (event.type == SDL_JOYDEVICEREMOVED) {
        int device_index = event.jdevice.which;
        zox_log("- Joystick Disconnected [%i]:[%s]", device_index,  SDL_JoystickNameForIndex(device_index));
        return 1;
    }
    return 0;
}
