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

SDL_WindowFlags zox_sdl_window_flags(byte fullscreen, byte maximized) {
    SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (fullscreen) {
        if (is_on_phosh()) {
            flags = flags | SDL_WINDOW_MAXIMIZED;
        } else {
            flags = flags | SDL_WINDOW_FULLSCREEN;
        }
#if zox_windows
        // SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "1");
        // SDL_SetHint(SDL_HINT_VIDEO_HIGH_DPI_DISABLED, "0");
#endif
    }
    if (fullscreen && maximized) {
#if !(defined(zox_windows) && defined(zox_sdl3))
        flags = flags | SDL_WINDOW_MAXIMIZED;
#endif
    }
    return flags;
}

void zox_sdl_gl_delete_context(SDL_GLContext context)
{
    SDL_GL_DestroyContext(context);
}

int zox_sdl_get_current_display_mode(int monitor, SDL_DisplayMode* mode) {
    int count = 0;
    SDL_DisplayID *displays = SDL_GetDisplays(&count);
    if (!displays || monitor < 0 || monitor >= count) {
        if (displays) {
            SDL_free(displays);
        }
        return -1;
    }
    const SDL_DisplayMode *sdl_mode = SDL_GetCurrentDisplayMode(displays[monitor]);
    if (!sdl_mode) {
        SDL_free(displays);
        return -1;
    }
    *mode = *sdl_mode;
    SDL_free(displays);
    return 0;
}

int zox_sdl_get_num_displays(void)
{
    int count = 0;
    SDL_DisplayID *displays = SDL_GetDisplays(&count);
    if (displays) {
        SDL_free(displays);
    }
    return count;
}

int zox_sdl_get_window_display(SDL_Window *window)
{
    SDL_DisplayID display = SDL_GetDisplayForWindow(window);
    return (int) display;
}

int zox_sdl_set_window_display_mode(SDL_Window* window, SDL_DisplayMode* mode) {
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
    zox_log(" > found [%i] render drivers\n", num_render_drivers)
    for (int i = 0; i < num_render_drivers; i++) {
        const char *name = zox_sdl_get_render_driver(i);
        if (name) {
            zox_log("     + render driver [%s]\n", name)
        }
    }
}

SDL_DisplayOrientation zox_sdl_get_display_orientation(int screen_index) {
    return SDL_GetCurrentDisplayOrientation((SDL_DisplayID) screen_index);
}

void zox_sdl_dispose_surface(SDL_Surface *surface) {
    SDL_DestroySurface(surface);
}

// --------------------------


byte sdl_event_quit(SDL_Event *event) {
    return event->type == SDL_EVENT_QUIT;
}

byte sdl_event_window_resized(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_RESIZED ||
        event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED;
}

byte sdl_event_display_orientation(SDL_Event *event) {
    return event->type == SDL_EVENT_DISPLAY_ORIENTATION;
}

byte sdl_event_window_moved(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_MOVED;
}

byte sdl_event_window_maximized(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_MAXIMIZED;
}

byte sdl_event_window_restored(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_RESTORED;
}

byte sdl_event_window_minimized(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_MINIMIZED;
}

byte sdl_event_window_focus_lost(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_FOCUS_LOST;
}

byte sdl_event_window_focus_gained(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_FOCUS_GAINED;
}

byte sdl_event_text_input(SDL_Event *event) {
    return event->type == SDL_EVENT_TEXT_INPUT;
}

int2 sdl_event_window_size(SDL_Event *event) {
    return (int2) {
        event->window.data1,
        event->window.data2
    };
}

byte sdl_event_display(SDL_Event *event) {
    return (byte) event->display.displayID;
}

const char *sdl_event_text(SDL_Event *event) {
    return event->text.text;
}