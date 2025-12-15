int initialize_video() {
    if (headless) {
        return EXIT_SUCCESS;
    }

    if (SDL_Init(SDL_INIT_VIDEO)) {
        zox_log_error("Error [SDL_INIT_VIDEO] [%s]", SDL_GetError());
        return EXIT_FAILURE;
    }
    zox_log("Success [SDL_INIT_VIDEO]");
    /*if (SDL_VideoInit(NULL)) {
        zox_log_error("failed to initialize sdl [%s]", SDL_GetError())
        return EXIT_FAILURE;
    }*/

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
