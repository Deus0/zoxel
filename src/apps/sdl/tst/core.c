// core sdl tests, the engine's base: without these nothing shows, nothing types, nothing closes
//  - sdl_window: the app has a window, its size is the app's WindowSize, an opengl context is current and answers
//  - sdl_resize: a resized event (as sdl sends) resizes the app (WindowSize), cameras and
//    canvases following it are cameras/tst (camera_viewports)
//  - sdl_quit: a quit event (closing the window) ends the main loop, the process exits cleanly (zox_tst_set_exits)
//  - run: bash bsh/tst.sh sdl_window (sdl_resize, sdl_quit)
int zox_tst_sdl_frames = 0;
int zox_tst_sdl_stage = 0;

void zox_tst_sdl_push(ecs* world, SDL_Event* event) {
    SDL_Window* window = zox_valid(main_app) ? zox_getv(main_app, SDLWindow) : NULL;
    event->window.windowID = window ? SDL_GetWindowID(window) : 0;
    SDL_PushEvent(event);
}

byte zox_tst_sdl_app_ready(ecs* world) {
    if (!zox_valid(main_app) || !zox_has(main_app, SDLWindow) || !zox_getv(main_app, SDLWindow)) {
        zox_tst_waiting = "the app's window";
        return 0;
    }
    return 1;
}

// sdl_window
byte zox_tst_sdl_window_ready(ecs* world) {
    if (!zox_tst_sdl_app_ready(world)) {
        return 0;
    }
    zox_tst_waiting = "a few frames";
    return ++zox_tst_sdl_frames >= 10;
}

void zox_tst_sdl_window_check(ecs* world) {
    SDL_Window* window = zox_getv(main_app, SDLWindow);
    zox_tst_check(window != NULL, "the app has no sdl window");
    if (!window) {
        return;
    }
    int2 size = int2_zero;
    SDL_GetWindowSize(window, &size.x, &size.y);
    const int2 app_size = zox_getv(main_app, WindowSize);
    zox_tst_check(size.x > 0 && size.y > 0, "the window has no size");
    zox_tst_check(int2_equals(size, app_size), "the window's size isn't the app's WindowSize");
    if (!int2_equals(size, app_size)) {
        zox_log("[tst] sdl window [%ix%i] app WindowSize [%ix%i]", size.x, size.y, app_size.x, app_size.y);
    }
    zox_tst_check(SDL_GL_GetCurrentWindow() == window, "the window's opengl context isn't current");
    zox_tst_check(SDL_GL_GetCurrentContext() != NULL, "no opengl context");
    const char* version = (const char*) glGetString(GL_VERSION);
    zox_tst_check(version && version[0], "opengl doesn't answer (no GL_VERSION)");
    if (version) {
        zox_log("[tst] opengl [%s]", version);
    }
}

// sdl_resize
int2 zox_tst_sdl_size_before, zox_tst_sdl_size_to;
byte zox_tst_sdl_resized = 0, zox_tst_sdl_resized_back = 0;

void zox_tst_sdl_push_resize(ecs* world, int2 size) {
    SDL_Event event = { 0 };
    event.type = SDL_EVENT_WINDOW_RESIZED;
    event.window.data1 = size.x;
    event.window.data2 = size.y;
    zox_tst_sdl_push(world, &event);
}

byte zox_tst_sdl_resize_ready(ecs* world) {
    if (!zox_tst_sdl_app_ready(world)) {
        return 0;
    }
    zox_tst_sdl_frames++;
    if (zox_tst_sdl_stage == 0) {
        zox_tst_waiting = "a few frames";
        if (zox_tst_sdl_frames < 10) {
            return 0;
        }
        zox_tst_sdl_size_before = zox_getv(main_app, WindowSize);
        zox_tst_sdl_size_to = (int2) { zox_tst_sdl_size_before.x - 100, zox_tst_sdl_size_before.y - 60 };
        zox_tst_sdl_push_resize(world, zox_tst_sdl_size_to);
        zox_tst_sdl_stage = 1;
        zox_tst_sdl_frames = 0;
        return 0;
    }
    if (zox_tst_sdl_stage == 1) {
        zox_tst_waiting = "the resize";
        if (zox_tst_sdl_frames < 10) {
            return 0;
        }
        zox_tst_sdl_resized = int2_equals(zox_getv(main_app, WindowSize), zox_tst_sdl_size_to);
        zox_tst_sdl_push_resize(world, zox_tst_sdl_size_before);
        zox_tst_sdl_stage = 2;
        zox_tst_sdl_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the resize back";
    if (zox_tst_sdl_frames < 10) {
        return 0;
    }
    zox_tst_sdl_resized_back = int2_equals(zox_getv(main_app, WindowSize), zox_tst_sdl_size_before);
    return 1;
}

void zox_tst_sdl_resize_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_sdl_resized, "a resized event didn't resize the app (WindowSize)");
    zox_tst_check(zox_tst_sdl_resized_back, "resized back, the app's WindowSize didn't follow");
}

// sdl_quit
byte zox_tst_sdl_quit_ready(ecs* world) {
    if (!zox_tst_sdl_app_ready(world)) {
        return 0;
    }
    if (zox_tst_sdl_stage == 0) {
        zox_tst_waiting = "a few frames";
        if (++zox_tst_sdl_frames < 10) {
            return 0;
        }
        SDL_Event event = { 0 };
        event.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&event);
        zox_tst_sdl_stage = 1;
    }
    zox_tst_waiting = "the app to close after a quit event";
    return 0;
}

void zox_tst_sdl_quit_check(ecs* world) {
    (void) world;
}
