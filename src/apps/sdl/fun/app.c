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

SDL_Surface* load_png_as_surface(const char *filepath) {
#ifndef zox_sdl_images
    return NULL;
#else
    return (SDL_Surface*) IMG_Load(filepath);
#endif
}

byte load_app_icon(SDL_Window* window, const char *icon_path) {
    if (!window) {
        return 0;
    }

#ifdef zox_sdl_images
    SDL_Surface *surface = load_png_as_surface(icon_path);
    if (surface) {
        SDL_SetWindowIcon(window, surface);
        SDL_FreeSurface(surface);
        zox_logv("Loaded Icon at [%s]", icon_path);
        return 1;
    } else {
        zox_log_error("Failed to load app icon [%s]", icon_path);
        return 0;
    }
#else
    return 0;
#endif
}

// extern byte load_app_icon(SDL_Window*, const char*);

byte spawn_window_icon(ecs *world, entity app, const char* texture_name) {

    if (!zox_valid(app) || !zox_has(app, SDLWindow)) {
        zox_log_error("App not spawned");
        return 1;
    }

    // Shaders
    char* path_textures = concat_file_path(resources_path, "textures");
    if (!path_textures) {
        zox_log_error("Textures Path not found.");
        return 1;
    }
    char* path_icon = concat_file_path(path_textures, texture_name);
    // zox_logv("Textures Path [%s]", path_textures);
    free(path_textures);
    if (!path_icon) {
        zox_log_error("Icon Path not found.");
        return 1;
    }

    zox_geter(app, SDLWindow, window);
    load_app_icon(window->value, path_icon);
    zox_logv("Set Icon [%s]", path_icon);
    free(path_icon);

    return 0;
}
