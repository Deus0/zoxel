int initialize_video() {
    if (headless) {
        zox_logw("Headless should not reach here.");
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
    zox_logv("Detected Video Drivers:");
    for (int i = 0; i < dcount; i++) {
        const char* dname = SDL_GetVideoDriver(i);
        zox_logv("   %d: %s", i + 1, dname);
    }
    if (!sdl_init(SDL_INIT_VIDEO)) {
        zox_loge("[SDL_INIT_VIDEO] [%s]", SDL_GetError());
        return EXIT_FAILURE;
    }
    const char* driver = SDL_GetCurrentVideoDriver();
    using_gpu = strstr(driver, "opengl") ||
        strstr(driver, "vulkan") ||
        strstr(driver, "wayland") ||
        strstr(driver, "x11") ||
        strstr(driver, "direct3d") ||
        strstr(driver, "metal") ||
        strstr(driver, "opengles") ||
        strstr(driver, "windows");
    if (!using_gpu) {
        zox_log("GPU Not Detected: %s", driver);
    }
    zox_logv("[SDL_INIT_VIDEO] %s - GPU [%i]", driver, using_gpu);
    print_sdl();
#ifdef zox_vulkan
    if (!load_vulkan_library()) {
        return EXIT_FAILURE;
    }
#endif
    set_sdl_attributes();
    screens_count = zox_sdl_get_num_displays();
    if (screens_count == 1) {
        screen_index = 0;
    }
    return EXIT_SUCCESS;
}

void close_sdl_video() {
    zox_sdl_video_quit();
}
