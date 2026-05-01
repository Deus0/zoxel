SDL_Window* create_sdl_window_basic_vulkan(
    const int2 position,
    const int2 size,
    const char *name
) {
    int flags = SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP;
#ifdef zox_android
    flags = flags | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE;
#endif
    SDL_Window *window = SDL_CreateWindow(name, position.x, position.y, size.x, size.y, flags);
    if (window == NULL) {
        zox_log("!!! SDL_CreateWindow failed: %s\n", SDL_GetError())
        return window;
    }
    return window;
}
