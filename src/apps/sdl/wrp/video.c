int initialize_video() {
    if (headless) {
        zox_logw("Headless should not reach here.");
        return EXIT_SUCCESS;
    }
    zox_logv("Wayland Decor [%i]", !disable_apps_decor);
    SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_ALLOW_LIBDECOR, !disable_apps_decor ? "1" : "0");
    int dcount = SDL_GetNumVideoDrivers();
    if (dcount < 1) {
        zox_log("No video drivers available!");
        return -1;
    }
    zox_logv("Detected Video Drivers:");
    for (int i = 0; i < dcount; i++) {
        zox_logv("   %d: %s", i + 1, SDL_GetVideoDriver(i));
    }
    if (!sdl_init(SDL_INIT_VIDEO)) {
        zox_loge("[SDL_INIT_VIDEO] [%s]", SDL_GetError());
        return EXIT_FAILURE;
    }
    set_sdl_attributes();
    // NOTE: Detects software rendering
    /*const char* renderer = (const char*) glGetString(GL_RENDERER);
    // const char* renderer = SDL_GetCurrentVideoDriver();
    using_gpu =
        renderer &&
        !strstr(renderer, "llvmpipe") &&
        !strstr(renderer, "softpipe") &&
        !strstr(renderer, "Software Rasterizer") &&
        !strstr(renderer, "SwiftShader");*/
    // NOTE: Detects software rendering
#ifdef zox_web
    using_gpu = 1;
#else
    const char* renderer = (const char*) glGetString(GL_RENDERER);
    using_gpu =
        renderer &&
        !strstr(renderer, "llvmpipe") &&
        !strstr(renderer, "softpipe") &&
        !strstr(renderer, "Software Rasterizer") &&
        !strstr(renderer, "SwiftShader");
    zox_logv("[SDL_INIT_VIDEO] GPU [%i] Renderer [%s]",
        using_gpu,
        renderer);
#endif
    print_sdl();
#ifdef zox_vulkan
    if (!load_vulkan_library()) {
        return EXIT_FAILURE;
    }
#endif
    screens_count = zox_sdl_get_num_displays();
    if (screens_count == 1) {
        screen_index = 0;
    }
    return EXIT_SUCCESS;
}

void close_sdl_video() {
    zox_sdl_video_quit();
}
