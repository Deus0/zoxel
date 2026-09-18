// NOTE: New SDL3 Display Helpers

// NOTE: If anyone has > 127 monitors, please submit a Ticket to Zoxel $,$
int zox_sdl_get_display_index(SDL_DisplayID display_id) {
    if (!display_id) {
        zox_loge("Invalid display_id [0]");
        return -1;
    }
    int count = 0;
    SDL_DisplayID *displays = SDL_GetDisplays(&count);
    if (!displays) {
        zox_loge("SDL_GetDisplays returns NULL [%s]", SDL_GetError());
        return -1;
    }
    for (int i = 0; i < count; i++) {
        if (displays[i] == display_id) {
            SDL_free(displays);
            return i;
        }
    }
    zox_logw("display_id [%u] not found in [%i]", display_id, count);
    for (int i = 0; i < count; i++) {
        zox_logw("  - display_id [%i]: [%u]", i, displays[i]);
    }
    SDL_free(displays);
    return -1;
}

void debug_sdl_display_event(SDL_Event *event) {
    zox_logw(
        "display event [%i] id [%u] data1 [%i] data2 [%i]",
        event->type,
        event->display.displayID,
        event->display.data1,
        event->display.data2
    );
}

SDL_DisplayID zox_sdl_get_display_id(byte monitor) {
    int count = 0;
    SDL_DisplayID *displays = SDL_GetDisplays(&count);
    if (!displays) {
        zox_loge("SDL_GetDisplays returns NULL [%s]", SDL_GetError());
        return 0;
    }
    if (monitor >= count) {
        zox_loge("Monitor [%i] OOB [%i]", monitor, count);
        SDL_free(displays);
        return 0;
    }
    SDL_DisplayID display_id = displays[monitor];
    SDL_free(displays);
    return display_id;
}

int zox_sdl_get_current_display_mode(int monitor, SDL_DisplayMode* mode) {
    SDL_DisplayID display_id = zox_sdl_get_display_id(monitor);
    if (!display_id) {
        return -1;
    }
    const SDL_DisplayMode *sdl_mode = SDL_GetCurrentDisplayMode(display_id);
    if (!sdl_mode) {
        zox_loge("SDL_GetCurrentDisplayMode returned NULL [%s]", SDL_GetError());
        return -1;
    }
    *mode = *sdl_mode;
    return 0;
}

int zox_sdl_get_window_display(SDL_Window *window) {
    return zox_sdl_get_display_index(SDL_GetDisplayForWindow(window));
}

int sdl_event_get_monitor(SDL_Event *event) {
    SDL_Window *window = SDL_GetWindowFromEvent(event);
    // SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
    if (!window) {
        // NOTE: Likely transient / unowned window
        // zox_logw("sdl_event_get_monitor NULL: type [%u] windowID [%u] display [%u] [%s]", event->type, event->window.windowID, event->display.displayID, SDL_GetError());
        return -1;
    }
    return zox_sdl_get_window_display(window);
    // debug_sdl_display_event(event);
    // int monitor = zox_sdl_get_display_index(event->display.displayID);
    // return monitor >= 0 ? (byte) monitor : 0;
}

byte sdl_init(uint flags) {
    return SDL_Init(flags);
}

byte sdl_init_subsystem(uint flags) {
    return SDL_InitSubSystem(flags);
}

byte sdl_gl_make_current(SDL_Window *window, SDL_GLContext context) {
    return SDL_GL_MakeCurrent(window, context);
}

void zox_sdl_video_quit(void) {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

SDL_Window *zox_sdl_create_window(
    const char *name,
    int x,
    int y,
    int w,
    int h,
    SDL_WindowFlags flags)
{
    SDL_Window *window = SDL_CreateWindow(name, w, h, flags);
    if (window && (x != SDL_WINDOWPOS_UNDEFINED || y != SDL_WINDOWPOS_UNDEFINED)) {
        SDL_SetWindowPosition(window, x, y);
    }
    return window;
}

void zox_sdl_gl_delete_context(SDL_GLContext context) {
    SDL_GL_DestroyContext(context);
}

int zox_sdl_get_num_displays() {
    int count = 0;
    SDL_DisplayID *displays = SDL_GetDisplays(&count);
    if (displays) {
        SDL_free(displays);
    }
    return count;
}

int zox_sdl_set_window_display_mode(SDL_Window* window, const SDL_DisplayMode* mode) {
    return SDL_SetWindowFullscreenMode(window, mode) ? 0 : -1;
}

void zox_sdl_show_cursor(byte visible) {
    if (visible) {
        SDL_ShowCursor();
    } else {
        SDL_HideCursor();
    }
}

const char* zox_sdl_get_render_driver(int index) {
    return SDL_GetRenderDriver(index);
}

int zox_sdl_get_version() {
    return SDL_GetVersion();
}

byte zox_sdl_opengl_es_supported() {
    byte is_supported = 0;
    int num_render_drivers = SDL_GetNumRenderDrivers();
    for (int i = 0; i < num_render_drivers; i++) {
        const char *name = SDL_GetRenderDriver(i);
        if (name && strstr(name, "opengles")) {
            is_supported = 1;
        }
    }
    return is_supported;
}

void zox_sdl_print_supported_renderers() {
    int num_render_drivers = SDL_GetNumRenderDrivers();
    zox_log("Found [%i] render drivers\n", num_render_drivers)
    for (int i = 0; i < num_render_drivers; i++) {
        const char *name = zox_sdl_get_render_driver(i);
        if (name) {
            zox_log(" + render driver [%s]\n", name)
        }
    }
}

SDL_DisplayOrientation zox_sdl_get_display_orientation(int screen_index) {
    SDL_DisplayID display_id = zox_sdl_get_display_id(monitor);
    if (!display_id) {
        return SDL_ORIENTATION_UNKNOWN;
    }
    return SDL_GetCurrentDisplayOrientation(display_id);
}

void zox_sdl_dispose_surface(SDL_Surface *surface) {
    SDL_DestroySurface(surface);
}

void on_set_vsync(byte value) {
    if (!SDL_GL_SetSwapInterval(value)) {
        zox_logw("Unable to disable VSync: [%s] - Value [%i]",
            SDL_GetError(),
            value);
    } else {
        zox_logv("VSync [%s]",
            vsync ?
                "Enabled" :
                "Disabled");
    }
}


// Screen

void zox_app_set_fullscreen(
    SDL_Window* window,
    byte monitor,
    byte fullscreen)
{
    if (fullscreen) {
        SDL_SetWindowFullscreenMode(window, NULL);
        if (!SDL_SetWindowFullscreen(window, true)) {
            zox_loge("SDL fullscreen error: %s", SDL_GetError());
        }
    } else {
        if (!SDL_SetWindowFullscreen(window, false)) {
            zox_loge("SDL windowed error: %s", SDL_GetError());
            return;
        }
        SDL_SyncWindow(window);
        SDL_SetWindowBordered(window, 1);
        SDL_ShowWindow(window);
        SDL_RaiseWindow(window);
    }
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    int x, y;
    SDL_GetWindowPosition(window, &x, &y);
    zox_log(
        "window fullscreen=%i size=%ix%i position=%i,%i",
        fullscreen, w, h, x, y);
}

byte sdl_window_is_minimized(SDL_Window *window) {
    return SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED;
}