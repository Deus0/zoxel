int initialize_video() {
    if (headless) {
        return EXIT_SUCCESS;
    }

    if (disable_apps_decor) {
        SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_ALLOW_LIBDECOR, "0");
    }

    int dcount = SDL_GetNumVideoDrivers();

    if (dcount < 1) {
        zox_log("No video drivers available!");
        return -1;
    }

    zox_log("Available video drivers:");
    for (int i = 0; i < dcount; i++) {
        const char* dname = SDL_GetVideoDriver(i);
        zox_log("%d: %s", i + 1, dname);
    }

    if (SDL_Init(SDL_INIT_VIDEO)) {
        zox_log_error("Error [SDL_INIT_VIDEO] [%s]", SDL_GetError());
        return EXIT_FAILURE;
    }

    const char* driver = SDL_GetCurrentVideoDriver();

    using_gpu = strstr(driver, "opengl") ||
    strstr(driver, "vulkan") ||
    strstr(driver, "wayland") ||
    strstr(driver, "direct3d") ||
    strstr(driver, "metal") ||
    strstr(driver, "opengles");

    zox_log("Initialized [SDL_INIT_VIDEO] %s - GPU [%i]", driver, using_gpu);

    if (is_log_sdl) {
        print_sdl();
    }

#ifdef zox_vulkan
    if (!load_vulkan_library()) {
        return EXIT_FAILURE;
    }
#endif

    set_sdl_attributes();
    screens_count = SDL_GetNumVideoDisplays();
    if (screens_count == 1) {
        screen_index = 0;
    }

    return EXIT_SUCCESS;
}

void close_sdl_video() {
    SDL_VideoQuit();
    // SDL_Quit();
}
