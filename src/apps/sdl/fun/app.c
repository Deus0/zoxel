void zox_set_app_fullscreen(ecs* world, entity e, byte fullscreen) {
    if (!zox_valid(e) || !zox_has(e, WindowFullscreen)) {
        zox_log_error("invalid app in [zox_set_app_fullscreen]");
        return;
    }
    zox_set(e, WindowFullscreen, { fullscreen });
    zox_geter(e, SDLWindow, window);
    zox_geter_value(e, WindowMonitor, byte, monitor);
    zox_app_set_fullscreen(window->value, monitor, fullscreen);
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
            zox_logv("+ maximizing app [%ix%i]", size.x, size.y);
            zox_app_set_size(world, e, size);
        } else {
            zox_geter_value(e, WindowSizeRestore, int2, restore_size);
            zox_geter_value(e, WindowPositionRestore, int2, restore_position);
            size = restore_size;
            position = restore_position;
            zox_logv("+ restoring app [%ix%i] at [%ix%i]", size.x, size.y, position.x, position.y);
            zox_app_set_size(world, e, size);
            zox_app_set_position(world, e, position);
        }
    }
}

byte load_app_icon(SDL_Window* window, const char *icon_path) {
    if (!icon_path) {
        zox_loge("Null [icon_path] in [load_app_icon]");
        return 0;
    }
    if (!window) {
        zox_loge("Null [SDL_Window] in [load_app_icon]");
        return 0;
    }
    SDL_Surface *surface = SDL_LoadBMP(icon_path);
    if (!surface) {
        zox_loge("Failed to load app icon '%s': %s",
                 icon_path, SDL_GetError());
        return 0;
    }
    SDL_SetWindowIcon(window, surface);
    SDL_FreeSurface(surface);
    zox_log("Loaded App Icon at [%s]", icon_path);
    return 1;
}

// SDL_Surface *surface = IMG_Load(icon_path); //  (SDL_Surface*)

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
