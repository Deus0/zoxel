static inline int2 get_screen_size() {
#ifdef zox_web
    return get_webasm_screen_size();
#else
    return get_screen_size_monitor(monitor);
#endif
}

int2 get_sdl_screen_size() {
    SDL_DisplayMode displayMode;
    if (zox_sdl_get_current_display_mode(0, &displayMode)) {
        zox_loge("[zox_sdl_get_current_display_mode] (get_sdl_screen_size) failed at %i: %s", 0, SDL_GetError());
        return (int2) { 480, 480 };
    }
    return (int2) { displayMode.w, displayMode.h };
}

void zox_app_set_maximized(SDL_Window* window, byte maximized) {
    if (maximized) {
        SDL_MaximizeWindow(window);
    } else {
        SDL_RestoreWindow(window);
    }
}

SDL_WindowFlags zox_sdl_window_flags(byte fullscreen, byte maximized) {
    SDL_WindowFlags flags =
        SDL_WINDOW_OPENGL |
        SDL_WINDOW_RESIZABLE;
        // SDL_WINDOW_SHOWN |
    if (fullscreen) {
        if (is_on_phosh()) {
            flags = flags | SDL_WINDOW_MAXIMIZED;
        } else {
            flags = flags | SDL_WINDOW_FULLSCREEN;
        }
    } else if (maximized) {
        flags |= SDL_WINDOW_MAXIMIZED;
    }
    return flags;
}
