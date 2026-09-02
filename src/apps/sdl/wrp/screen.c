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

/*void zox_app_set_fullscreen(SDL_Window* window, byte monitor, byte fullscreen) {
    SDL_SetWindowFullscreen(window, fullscreen ? sdl_fullscreen_byte : 0);
    // This was to restore monitor position
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
    } else {
        SDL_ShowWindow(window);
        SDL_RaiseWindow(window);
    }
}*/

void zox_app_set_fullscreen(
    SDL_Window* window,
    byte monitor,
    byte fullscreen)
{
    if (fullscreen) {
        int max_monitors = zox_sdl_get_num_displays();
        if (monitor >= max_monitors) {
            zox_loge("Invalid monitor index %i, using primary (0)", monitor);
            monitor = 0;
        }
        int monitor_current = zox_sdl_get_window_display(window);
        if (!(monitor_current >= 0 && monitor_current < max_monitors)) {
            zox_loge("monitor_current is oob [%i] of [%i]", monitor_current, max_monitors);
        } else if (monitor != monitor_current) {
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
    byte flag = fullscreen ?
        SDL_WINDOW_FULLSCREEN :
        0;
    if (!SDL_SetWindowFullscreen(window, fullscreen)) {
        zox_loge("SDL Fullscreen error: %s",
            SDL_GetError());
    }
    if (!fullscreen) {
        SDL_SetWindowBordered(window, 1);
    }
    if (!SDL_SyncWindow(window)) {
        zox_loge("SDL Sync error: %s",
            SDL_GetError());
    }
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
