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

void input_extract_from_sdl(ecs *world, const SDL_Event event) {
    sdl_extract_keyboard(world, event);
    sdl_extract_mouse_wheel(event);
    if (event.type == SDL_JOYDEVICEADDED) {
        handle_new_sdl_gamepad(world, event);
    }
}
