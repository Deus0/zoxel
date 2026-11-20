// a monitor's bounds (zox_sdl_get_display_bounds, sdl2 and sdl3 differ), the first's if it has none
SDL_Rect get_monitor_bounds(byte index) {
    SDL_Rect b = { 0 };
    if (!zox_sdl_get_display_bounds(index, &b)) {
        zox_loge("[get_monitor_bounds] failed for [%d]: %s", index, SDL_GetError());
        if (index == 0) {
            return (SDL_Rect) { .w = 480, .h = 480 };
        } else {
            return get_monitor_bounds(0);
        }
    }
    return b;
}

int2 calculate_monitor_position(byte index, byte center_window, int2 ws) {
    SDL_Rect b = get_monitor_bounds(index);
    int2 position = (int2) { b.x, b.y };
    if (center_window) {
        position.x += (b.w - ws.x) / 2;
        position.y += (b.h - ws.y) / 2;
    }
    return position;
}

byte zox_app_set_monitor(SDL_Window *window, byte index, byte center_window) {
    if (!window) {
        return 0;
    }
    int2 ws;
    SDL_GetWindowSize(window, &ws.x, &ws.y);
    const int2 position = calculate_monitor_position(index, center_window, ws);
    SDL_SetWindowPosition(window, position.x, position.y);
    if (is_log_monitors) {
        zox_log(" + zox_app_set_monitor [%i] at [%ix%i]", index, position.x, position.y)
        for (int i = 0; i < zox_sdl_get_num_displays(); i++) {
            SDL_Rect b2;
            if (zox_sdl_get_display_bounds(i, &b2)) {
                zox_log(" - screen [%i] bounds [%ix%i - %ix%i]", i, b2.x, b2.y, b2.w, b2.h)
            }
        }
    }
    return 1;
}

int2 get_screen_size_monitor(byte monitor) {
    SDL_DisplayMode displayMode;
    if (zox_sdl_get_current_display_mode(monitor, &displayMode)) {
        zox_loge("[SDL_GetCurrentDisplayMode] (get_screen_size_monitor) failed at %i: %s", monitor, SDL_GetError());
        if (monitor == 0) {
            return (int2) { 480, 480 };
        } else {
            return get_screen_size_monitor(0);
        }
    }
    // zox_logv("Monitor Screen Size [%i] [%ix%i]", monitor, displayMode.w, displayMode.h);
    return (int2) { displayMode.w, displayMode.h };
}
