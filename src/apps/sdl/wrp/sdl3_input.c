void zox_sdl_start_text_input(SDL_Window* window) {
    SDL_StartTextInput(window);
}

void zox_sdl_stop_text_input(SDL_Window* window) {
    SDL_StopTextInput(window);
}

byte zox_sdl_text_input_active(SDL_Window* window) {
    return SDL_TextInputActive(window);
}

byte zox_sdl_screen_keyboard_shown(SDL_Window* window) {
    return SDL_ScreenKeyboardShown(window);
}

void zox_sdl_set_relative_mouse_mode(SDL_Window *window, byte enabled) {
    SDL_SetWindowRelativeMouseMode(window, enabled);
}

void sdl_warp_mouse_in_window(SDL_Window *window, int x, int y) {
    SDL_WarpMouseInWindow(window, (float) x, (float) y);
}

const byte* sdl_get_keyboard_state() {
    return (const byte*) SDL_GetKeyboardState(NULL);
}

uint zox_sdl_get_mouse_state(int2 *position) {
    float x;
    float y;
    uint state = SDL_GetMouseState(&x, &y);
    position->x = (int) x;
    position->y = (int) y;
    return state;
}