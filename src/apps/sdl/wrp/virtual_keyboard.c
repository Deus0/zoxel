void disable_virtual_keyboard() {
    SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "0");
}

void enable_virtual_keyboard() {
    SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "1");
}

void show_virtual_keyboard(SDL_Window* window) {
    zox_sdl_start_text_input(window);
}

byte can_virtual_keyboard() {
    return SDL_HasScreenKeyboardSupport();
}

void hide_virtual_keyboard(SDL_Window* window) {
    zox_sdl_stop_text_input(window);
}

byte is_virtual_keyboard_shown(SDL_Window* window) {
    // if (SDL_IsScreenKeyboardShown(window)) zox_log("Screen Keyboard Shown");
    return zox_sdl_text_input_active(window) || zox_sdl_screen_keyboard_shown(window);
}

void toggle_virtual_keyboard(SDL_Window* window) {
    if (zox_sdl_text_input_active(window)) {
        hide_virtual_keyboard(window);
    } else {
        show_virtual_keyboard(window);
    }
}