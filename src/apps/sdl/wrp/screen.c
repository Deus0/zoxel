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

void zox_app_set_fullscreen(SDL_Window* window, byte monitor, byte fullscreen) {
    byte flag = fullscreen ? sdl_fullscreen_byte : 0;
    // zox_log("# fullscreen flag [%i]", flag)
    if (fullscreen) {
        int display_count = zox_sdl_get_num_displays();
        if (monitor >= display_count) {
            zox_loge("Invalid monitor index %i, using primary (0)", monitor);
            monitor = 0;
        }
        int monitor_current = zox_sdl_get_window_display(window);
        if (monitor_current >= 0 && monitor != monitor_current) {
            zox_log("Window moved monitors [%i] => [%i]", monitor, monitor_current);
            monitor = monitor_current;
        }
        SDL_DisplayMode display_mode;
        if (zox_sdl_get_current_display_mode(monitor, &display_mode)) {
            zox_loge("Failed getting display mode in [zox_app_set_fullscreen]");
        } else {
            zox_sdl_set_window_display_mode(window, &display_mode);
        }
    }
    SDL_SetWindowFullscreen(window, flag);
}

void zox_app_set_maximized(SDL_Window* window, byte maximized) {
    if (maximized) {
        SDL_MaximizeWindow(window);
    } else {
        SDL_RestoreWindow(window);
    }
}
