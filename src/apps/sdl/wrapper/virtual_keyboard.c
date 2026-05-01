void disable_virtual_keyboard() {
    SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "0");
}

void enable_virtual_keyboard() {
    SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "1");
}

void show_virtual_keyboard() {
    zox_log("Showing Virtual Keyboard");
    SDL_StartTextInput();
}

void hide_virtual_keyboard() {
    SDL_StopTextInput();
}

byte can_virtual_keyboard() {
    return SDL_HasScreenKeyboardSupport();
}

byte is_virtual_keyboard_shown(SDL_Window* window) {
    // if (SDL_IsScreenKeyboardShown(window)) zox_log("Screen Keyboard Shown");
    return SDL_IsTextInputActive() || SDL_IsScreenKeyboardShown(window);
}

void toggle_virtual_keyboard() {
    if (SDL_IsTextInputActive()) {
        hide_virtual_keyboard();
    } else {
        show_virtual_keyboard();
    }
}