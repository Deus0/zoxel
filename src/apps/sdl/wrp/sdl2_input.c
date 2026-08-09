void zox_sdl_start_text_input(SDL_Window* window) {
    SDL_StartTextInput();
}

void zox_sdl_stop_text_input(SDL_Window* window) {
    SDL_StopTextInput();
}

byte zox_sdl_text_input_active(SDL_Window* window) {
    return SDL_IsTextInputActive();
}

byte zox_sdl_screen_keyboard_shown(SDL_Window* window) {
    return SDL_IsScreenKeyboardShown(window);
}

void zox_sdl_set_relative_mouse_mode(SDL_Window *window, byte enabled) {
    SDL_SetRelativeMouseMode(enabled);
}

void sdl_warp_mouse_in_window(SDL_Window *window, int x, int y) {
    SDL_WarpMouseInWindow(window, x, y);
}

const byte* sdl_get_keyboard_state() {
    return SDL_GetKeyboardState(NULL);
}

uint zox_sdl_get_mouse_state(int2* position) {
    return SDL_GetMouseState(&position->x, &position->y);
}