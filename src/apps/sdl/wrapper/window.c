void zox_sdl_window_size( SDL_Window* sdl_window, int2 size) {
    SDL_SetWindowSize(sdl_window, size.x, size.y);
}

void zox_app_set_size(ecs *world, entity e, int2 size) {
    zox_geter_value_non_const(e, SDLWindow, SDL_Window*, sdl_window);
    zox_sdl_window_size(sdl_window, size);
    if (!int2_equals(size, zox_gett_value(e, WindowSize))) {
        zox_set(e, WindowSize, { size })
        zox_set(e, WindowSizeDirty, { zox_dirty_trigger })
    }
}

void zox_app_set_position(ecs *world, entity e, int2 position) {
    zox_geter_value_non_const(e, SDLWindow, SDL_Window*, sdl_window)
    SDL_SetWindowPosition(sdl_window, position.x, position.y);
    zox_set(e, WindowPosition, { position })
}

int get_sdl_window_header_size(ecs* world, entity e) {
    zox_geter_value_non_const(e, SDLWindow, SDL_Window*, sdl_window)
    int top, left, bottom, right;
    if (!SDL_GetWindowBordersSize(sdl_window, &top, &left, &bottom, &right)) {
        return top;
    } else {
        return 0;
    }
}

int2 get_window_size_without_header(ecs* world, entity e, int2 window_size) {
    return int2_sub(window_size, (int2) { 0, get_sdl_window_header_size(world, e) });
}

// this should... account for taskbar too?
int2 get_maximized_size(ecs* world, entity e) {
    return int2_sub(get_screen_size(), (int2) { 0, get_sdl_window_header_size(world, e) });
}

SDL_Window* create_sdl_window(int2 position, int2 size, const char *name, byte flags) {
    SDL_Window *window = SDL_CreateWindow(name, position.x, position.y, size.x, size.y, flags);
    if (!window) {
        zox_log_error(" CreateWindowError [%s] - flags [%i]]\n", SDL_GetError(), flags);
        return NULL;
    }
    zox_logv("Created SDL Window [%s]", name);
    SDL_SetWindowResizable(window, window_resizeable);
    return window;
}

void on_set_vsync(byte value) {
    if (SDL_GL_SetSwapInterval(value)) {
        zox_logw("Unable to disable VSync: %s", SDL_GetError());
    } else {
        zox_logv("VSync [%s]", vsync ? "Enabled" : "Disabled");
    }
}

/*if (screen_index == 1) {
    SDL_Rect displayBounds;
    SDL_GetDisplayBounds(1, &displayBounds);
    int xPos, yPos;
    SDL_GetWindowPosition(window, &xPos, &yPos);
    int borderWidth, borderHeight;
    SDL_GetWindowBordersSize(window, &borderWidth, NULL, &borderHeight, NULL);
    xPos += borderWidth;
    yPos += borderHeight;
    SDL_SetWindowPosition(window, displayBounds.x + xPos, displayBounds.y + yPos);
}*/
