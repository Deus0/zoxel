void zox_app_set_position(ecs *world, entity e, int2 position) {
    SDL_Window* sdl_window = zox_getv(e, SDLWindow);
    SDL_SetWindowPosition(sdl_window, position.x, position.y);
    zox_setv(e, WindowPosition, position);
}


int get_sdl_window_header_size(ecs* world, entity e) {
    SDL_Window* sdl_window = zox_getv(e, SDLWindow);
    int top, left, bottom, right;
    if (!SDL_GetWindowBordersSize(sdl_window, &top, &left, &bottom, &right)) {
        return top;
    } else {
        return 0;
    }
}

int2 get_window_size_without_header(ecs* world, entity e, int2 window_size) {
    return int2_sub(window_size, (int2) { 0, get_sdl_window_header_size(world, e) });
}

// this should... account for taskbar too?
int2 get_maximized_size(ecs* world, entity e) {
    return int2_sub(get_screen_size(), (int2) { 0, get_sdl_window_header_size(world, e) });
}

void zox_app_set_monitor_e(ecs *world, entity e, byte monitor) {
    SDL_Window* sdl_window = zox_getv(e, SDLWindow);
    zox_app_set_monitor(sdl_window, monitor, 1);
    zox_set(e, WindowMonitor, { monitor });
}

void zox_app_set_size(ecs *world, entity e, int2 size) {
    SDL_Window* sdl_window = zox_getv(e, SDLWindow);
    zox_sdl_window_size(sdl_window, size);
    if (!int2_equals(size, zox_getv(e, WindowSize))) {
        zox_set(e, WindowSize, { size })
        zox_set(e, WindowSizeDirty, { zox_dirty_trigger })
    }
}

void on_sdl_window_restored(ecs *world, entity e) {
    if (!zox_has(e, WindowSize)) {
        zox_loge("invalid app [%lu]", e);
        return;
    }
    SDL_Window* sdl_window = zox_getv(e, SDLWindow);
    int2 size = zox_getv(e, WindowSizeRestore);
    int2 position = zox_getv(e, WindowPositionRestore);
    if (size.x == 0 && size.y == 0) {
        int2 screen_size = get_screen_size();
        size.x = screen_size.x / 2;
        size.y = screen_size.y / 2;
    }
    SDL_SetWindowSize(sdl_window, size.x, size.y);
    SDL_SetWindowPosition(sdl_window, position.x, position.y);
    zox_set(e, WindowSize, { size })
    zox_set(e, WindowSizeDirty, { zox_dirty_trigger })
    zox_logv(" > setting to window: position [%ix%i] size [%ix%i]", position.x, position.y, size.x, size.y);
}

void zox_set_app_fullscreen(ecs* world, entity e, byte fullscreen) {
    if (!zox_valid(e) || !zox_has(e, WindowFullscreen)) {
        zox_loge("invalid app in [zox_set_app_fullscreen]");
        return;
    }
    zox_setv(e, WindowFullscreen, fullscreen);
    byte monitor = zox_getv(e, WindowMonitor);
    const SDLWindow* window = zox_get(e, SDLWindow);
    zox_app_set_fullscreen(window->value, monitor, fullscreen);
}

// sdl implementation for maximized state
void zox_set_app_maximized(ecs* world, entity e, byte maximized) {
    if (!zox_valid(e) || !zox_has(e, WindowFullscreen)) {
        zox_log_error("invalid app in [zox_set_app_fullscreen]")
        return;
    }
    byte fullscreen = zox_getv(e, WindowFullscreen);
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
            int2 restore_size = zox_getv(e, WindowSizeRestore);
            int2 restore_position = zox_getv(e, WindowPositionRestore);
            size = restore_size;
            position = restore_position;
            zox_logv("+ restoring app [%ix%i] at [%ix%i]", size.x, size.y, position.x, position.y);
            zox_app_set_size(world, e, size);
            zox_app_set_position(world, e, position);
        }
    }
}

static inline byte load_app_icon(SDL_Window* window, const char *icon_path) {
    if (!window || !icon_path) {
        zox_loge("Invalid arguments in [load_app_icon]");
        return 0;
    }
    SDL_Surface *surface = SDL_LoadBMP(icon_path);
    if (!surface) {
        zox_loge("Failed to load app icon '%s': %s", icon_path, SDL_GetError());
        return 0;
    }
#ifdef zox_sdl3
    if (!SDL_SetWindowIcon(window, surface)) {
        zox_loge("SDL_SetWindowIcon failed: %s", SDL_GetError());
        zox_sdl_dispose_surface(surface);
        return 0;
    }
#else
    SDL_SetWindowIcon(window, surface);
#endif
    zox_sdl_dispose_surface(surface);
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
