void zox_sdl_window_size( SDL_Window* sdl_window, int2 size) {
    SDL_SetWindowSize(sdl_window, size.x, size.y);
}

SDL_Window* create_sdl_window(int2 position, int2 size, const char *name, SDL_WindowFlags flags) {
    SDL_Window *window = zox_sdl_create_window(name, position.x, position.y, size.x, size.y, flags);
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
    // zox_log("SDL_GL_SwapInterval: %d", SDL_GL_GetSwapInterval());
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
