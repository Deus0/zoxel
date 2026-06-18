void zox_set_app_fullscreen(ecs* world, entity e, byte fullscreen) {
    if (!zox_valid(e) || !zox_has(e, WindowFullscreen)) {
        zox_log_error("invalid app in [zox_set_app_fullscreen]");
        return;
    }
    zox_set(e, WindowFullscreen, { fullscreen });
    zox_geter(e, SDLWindow, window);
    zox_geter_value(e, WindowMonitor, byte, monitor);
    zox_app_set_fullscreen(window->value, monitor, fullscreen);
    // zox_log("Setting Viewport [%s]", fullscreen ? "fullscreen" : "windowed");
}

// sdl implementation for maximized state
void zox_set_app_maximized(ecs* world, entity e, byte maximized) {
    if (!zox_valid(e) || !zox_has(e, WindowFullscreen)) {
        zox_log_error("invalid app in [zox_set_app_fullscreen]")
        return;
    }
    zox_geter_value(e, WindowFullscreen, byte, fullscreen);
    zox_set(e, WindowMaximized, { maximized });
    if (!fullscreen) {
        int2 size;
        int2 position;
        if (maximized) {
            size = get_maximized_size(world, e);
            // position = int2_half(get_screen_size());
            zox_log_sdl("+ maximizing app [%ix%i]", size.x, size.y)
            zox_app_set_size(world, e, size);
        } else {
            zox_geter_value(e, WindowSizeRestore, int2, restore_size);
            zox_geter_value(e, WindowPositionRestore, int2, restore_position);
            size = restore_size;
            position = restore_position;
            zox_log_sdl("+ restoring app [%ix%i] at [%ix%i]", size.x, size.y, position.x, position.y);
            zox_app_set_size(world, e, size);
            zox_app_set_position(world, e, position);
        }
    }
}

byte load_app_icon(SDL_Window* window, const char *icon_path) {
    if (!window) {
        zox_loge("Null SDL_Window");
        return 0;
    }
#ifdef zox_sdl_images
    SDL_Surface *surface = SDL_LoadBMP(icon_path);
    // SDL_Surface *surface = IMG_Load(icon_path); //  (SDL_Surface*)
    if (!surface) {
        zox_loge("App Icon Failure at [%s]: [%s]", icon_path, IMG_GetError());
        return 0;
    }
    zox_log("Loaded App Icon at [%s]", icon_path);
    SDL_SetWindowIcon(window, surface);
    SDL_FreeSurface(surface);
    return 1;
#else
    zox_logw("No SDL_Image cannot load icon [%s]", icon_path);
    return 0;
#endif
}

// extern byte load_app_icon(SDL_Window*, const char*);

byte spawn_window_icon(ecs *world, entity app, const char* texture_name) {
    if (!zox_valid(app) || !zox_has(app, SDLWindow)) {
        zox_loge("App not spawned");
        return 1;
    }
    // Shaders
    char* path_textures = concat_file_path(resources_path, "textures");
    if (!path_textures) {
        zox_loge("Textures Path not found.");
        return 1;
    }
    char* icon_filepath = concat_file_path(path_textures, texture_name);
    free(path_textures);
    if (!icon_filepath) {
        zox_log_error("Icon Path not found.");
        return 1;
    }
    zox_geter(app, SDLWindow, window);
    load_app_icon(window->value, icon_filepath);
    zox_logv("Set Icon [%s]", icon_filepath);
    free(icon_filepath);
    return 0;
}
