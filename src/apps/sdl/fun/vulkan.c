// the window's vulkan (sdl3): the instance with the window's extensions, its surface, the device
//  - --background runs (tests) draw to an offscreen image, the window gets no surface
#ifdef zox_vulkan
byte spawn_vulkan_on_window(SDL_Window* window) {
    int2 size;
    SDL_GetWindowSizeInPixels(window, &size.x, &size.y);
    Uint32 count = 0;
    const char* const* extensions = NULL;
    if (!background) {
        extensions = SDL_Vulkan_GetInstanceExtensions(&count);
        if (!extensions) {
            zox_loge("[SDL_Vulkan_GetInstanceExtensions] [%s]", SDL_GetError());
            return 0;
        }
    }
    if (!vulkan_create_instance(extensions, count)) {
        return 0;
    }
    if (!background && !SDL_Vulkan_CreateSurface(window, vk_instance, NULL, &vk_surface)) {
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
