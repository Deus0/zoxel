// the window's vulkan: the instance with the window's extensions, its surface, the device
//  - --background runs (tests) draw to an offscreen image, the window gets no surface
//  - sdl3 and sdl2 (windows keeps sdl2 for its windowing) differ only in the calls below
#ifdef zox_vulkan

// the extensions a surface on this window needs, NULL if sdl can't say
const char* const* zox_sdl_vulkan_extensions(SDL_Window* window, uint* count) {
#ifdef zox_sdl3
    (void) window;
    return SDL_Vulkan_GetInstanceExtensions(count);
#else
    static const char* names[16];
    *count = 16;
    return SDL_Vulkan_GetInstanceExtensions(window, count, names) ? names : NULL;
#endif
}

byte zox_sdl_vulkan_surface(SDL_Window* window) {
#ifdef zox_sdl3
    return SDL_Vulkan_CreateSurface(window, vk_instance, NULL, &vk_surface);
#else
    return SDL_Vulkan_CreateSurface(window, vk_instance, &vk_surface);
#endif
}

byte spawn_vulkan_on_window(SDL_Window* window) {
    int2 size;
#ifdef zox_sdl3
    SDL_GetWindowSizeInPixels(window, &size.x, &size.y);
#else
    SDL_Vulkan_GetDrawableSize(window, &size.x, &size.y);
#endif
    uint count = 0;
    const char* const* extensions = NULL;
    if (!background) {
        extensions = zox_sdl_vulkan_extensions(window, &count);
        if (!extensions) {
            zox_loge("[SDL_Vulkan_GetInstanceExtensions] [%s]", SDL_GetError());
            return 0;
        }
    }
    vk_vsync = vsync;
    if (!vulkan_create_instance(extensions, count)) {
        return 0;
    }
    if (!background && !zox_sdl_vulkan_surface(window)) {
        zox_loge("[SDL_Vulkan_CreateSurface] [%s]", SDL_GetError());
        return 0;
    }
    if (!vulkan_initialize(size)) {
        return 0;
    }
    using_gpu = vk_on_gpu;
    return 1;
}
#endif
