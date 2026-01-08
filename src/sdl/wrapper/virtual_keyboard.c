void disable_virtual_keyboard() {
    SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "0");
}

void enable_virtual_keyboard() {
    SDL_SetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD, "1");
}

void show_virtual_keyboard() {
    SDL_StartTextInput();
}

void hide_virtual_keyboard() {
    SDL_StopTextInput();
}