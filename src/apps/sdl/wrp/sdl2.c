byte sdl_init(uint flags) {
    return SDL_Init(flags) == 0;
}

byte sdl_init_subsystem(uint flags) {
    return SDL_InitSubSystem(flags) == 0;
}

byte sdl_gl_make_current(SDL_Window *window, SDL_GLContext context) {
    return SDL_GL_MakeCurrent(window, context) == 0;
}

void zox_sdl_video_quit(void) {
    SDL_VideoQuit();
}

SDL_Window *zox_sdl_create_window(
    const char *name,
    int x,
    int y,
    int w,
    int h,
    SDL_WindowFlags flags)
{
    return SDL_CreateWindow(name, x, y, w, h, flags);
}

SDL_WindowFlags zox_sdl_window_flags(byte fullscreen, byte maximized) {
    SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (fullscreen) {
        if (is_on_phosh()) {
            flags = flags | SDL_WINDOW_MAXIMIZED;
        } else {
            flags = flags | SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
#if zox_windows
        SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "1");
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
#endif
    }
    if (fullscreen && maximized) {
        flags = flags | SDL_WINDOW_MAXIMIZED;
    }
    return flags;
}

void zox_sdl_gl_delete_context(SDL_GLContext context)
{
    SDL_GL_DeleteContext(context);
}

int zox_sdl_get_current_display_mode(int monitor, SDL_DisplayMode* mode) {
    return SDL_GetCurrentDisplayMode(monitor, mode);
}

int zox_sdl_get_num_displays()
{
    return SDL_GetNumVideoDisplays();
}

int zox_sdl_get_window_display(SDL_Window *window)
{
    return SDL_GetWindowDisplayIndex(window);
}

void zox_sdl_show_cursor(byte visible) {
    SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

int zox_sdl_set_window_display_mode(SDL_Window* window, SDL_DisplayMode* mode) {
    return SDL_SetWindowDisplayMode(window, mode);
}

const char* zox_sdl_get_render_driver(int index) {
    SDL_RendererInfo info;
    if (SDL_GetRenderDriverInfo(index, &info) != 0) {
        return NULL;
    }
    return info.name;
}

int zox_sdl_get_version(void) {
    SDL_version version;
    SDL_GetVersion(&version);
    return SDL_VERSIONNUM(version.major, version.minor, version.patch);
}

byte zox_sdl_opengl_es_supported() {
    byte is_supported = 0;
    int num_render_drivers = SDL_GetNumRenderDrivers();
    for (int i = 0; i < num_render_drivers; i++) {
        SDL_RendererInfo info;
        if (SDL_GetRenderDriverInfo(i, &info) != 0) {
            continue;
        }
        if (strstr(info.name, "opengles")) {
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
    return SDL_GetDisplayOrientation(screen_index);
}

void zox_sdl_dispose_surface(SDL_Surface *surface) {
    SDL_FreeSurface(surface);
}

// ----------------------------------------

byte sdl_event_quit(SDL_Event *event) {
    return event->type == SDL_QUIT;
}

byte sdl_event_window_resized(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        (event->window.event == SDL_WINDOWEVENT_RESIZED ||
         event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED);
}

byte sdl_event_display_orientation(SDL_Event *event) {
    return event->type == SDL_DISPLAYEVENT &&
        event->display.event == SDL_DISPLAYEVENT_ORIENTATION;
}

byte sdl_event_window_moved(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_MOVED;
}

byte sdl_event_window_maximized(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_MAXIMIZED;
}

byte sdl_event_window_restored(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_RESTORED;
}

byte sdl_event_window_minimized(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_MINIMIZED;
}

byte sdl_event_window_focus_lost(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_FOCUS_LOST;
}

byte sdl_event_window_focus_gained(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_FOCUS_GAINED;
}

byte sdl_event_text_input(SDL_Event *event) {
    return event->type == SDL_TEXTINPUT;
}

int2 sdl_event_window_size(SDL_Event *event) {
    return (int2) {
        event->window.data1,
        event->window.data2
    };
}

byte sdl_event_display(SDL_Event *event) {
    return (byte) event->display.display;
}

const char *sdl_event_text(SDL_Event *event) {
    return event->text.text;
}