int initialize_video() {
    if (headless) {
        return EXIT_SUCCESS;
    }

    if (disable_apps_decor) {
        SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_ALLOW_LIBDECOR, "0");
    }

    if (SDL_Init(SDL_INIT_VIDEO)) {
        zox_log_error("Error [SDL_INIT_VIDEO] [%s]", SDL_GetError());
        return EXIT_FAILURE;
    }

    zox_logv("Initialized [SDL_INIT_VIDEO]");

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
