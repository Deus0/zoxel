// tst: sdl_window_mode, fullscreen and back to windowed (sdl3), twice, as the settings' Fullscreen does
//  (zox_set_app_fullscreen): windowed again, the window is there to use
//  - not fullscreen, not minimized, not hidden (with --visible: tests are hidden), bordered (its title bar)
//  - as the app was: maximized (WindowMaximized), or its restore size (WindowSizeRestore); on a display (its middle
//    inside one's bounds)
//  - windows: the os agrees (IsWindowVisible, not IsIconic, WS_CAPTION), sdl's flags can say one thing and win32 another
//  - run: bash bsh/tst.sh sdl_window_mode --visible
#define zox_tst_wm_toggles 4     // fullscreen, windowed, fullscreen, windowed
#define zox_tst_wm_frames 30     // a mode change is asynchronous, the os takes a few frames: at most these
#define zox_tst_wm_settle 4      // frames after the window is in its mode, before checking or the next toggle
int zox_tst_wm_stage = 0;
int zox_tst_wm_frames_in = 0;
int zox_tst_wm_fails_at = -1;   // the first windowed stage that failed, logged

// is the window usable windowed, logs why not
byte zox_tst_wm_windowed_ok(ecs* world, SDL_Window* window, int toggle) {
    SDL_SyncWindow(window);
    const SDL_WindowFlags flags = SDL_GetWindowFlags(window);
    int2 size = int2_zero, position = int2_zero;
    SDL_GetWindowSize(window, &size.x, &size.y);
    SDL_GetWindowPosition(window, &position.x, &position.y);
    const int2 restore = zox_getv(main_app, WindowSizeRestore);
    const float2 middle = { position.x + size.x / 2.0f, position.y + size.y / 2.0f };
    byte on_display = 0;
    int displays_count = 0;
    SDL_DisplayID* displays = SDL_GetDisplays(&displays_count);
    for (int i = 0; displays && i < displays_count; i++) {
        SDL_Rect bounds;
        if (SDL_GetDisplayBounds(displays[i], &bounds)) {
            on_display |= middle.x >= bounds.x && middle.x < bounds.x + bounds.w &&
                middle.y >= bounds.y && middle.y < bounds.y + bounds.h;
        }
    }
    SDL_free(displays);
    zox_log("[tst] windowed [%i]: flags [%llx] size [%ix%i] (restore [%ix%i]) at [%ix%i], on a display [%i]", toggle,
        (unsigned long long) flags, size.x, size.y, restore.x, restore.y, position.x, position.y, on_display);
    const byte fullscreen = (flags & SDL_WINDOW_FULLSCREEN) != 0;
    const byte minimized = (flags & SDL_WINDOW_MINIMIZED) != 0;
    const byte hidden = (flags & SDL_WINDOW_HIDDEN) != 0 && !background;
    const byte borderless = (flags & SDL_WINDOW_BORDERLESS) != 0;
    zox_tst_check(!fullscreen, "windowed [%i], still fullscreen", toggle);
    zox_tst_check(!minimized, "windowed [%i], minimized", toggle);
    zox_tst_check(!hidden, "windowed [%i], hidden", toggle);
    zox_tst_check(!borderless, "windowed [%i], borderless (no title bar)", toggle);
    zox_tst_check(size.x > 0 && size.y > 0, "windowed [%i], no size [%ix%i]", toggle, size.x, size.y);
    // as the app was before: maximized, or its restore size
    const byte app_maximized = zox_getv(main_app, WindowMaximized);
    const byte maximized = (flags & SDL_WINDOW_MAXIMIZED) != 0;
    const byte sized = app_maximized ? maximized : int2_equals(size, restore);
    if (app_maximized) {
        zox_tst_check(maximized, "windowed [%i], the app is maximized but its window isn't, size [%ix%i]", toggle,
            size.x, size.y);
    } else {
        zox_tst_check(sized, "windowed [%i], size [%ix%i] not the restore size [%ix%i]", toggle,
            size.x, size.y, restore.x, restore.y);
    }
    zox_tst_check(on_display, "windowed [%i], off every display at [%ix%i]", toggle, position.x, position.y);
    byte os_ok = 1;
#ifdef zox_windows
    HWND hwnd = (HWND) SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (hwnd) {
        const LONG style = GetWindowLongA(hwnd, GWL_STYLE);
        RECT rect;
        GetWindowRect(hwnd, &rect);
        zox_log("[tst] windowed [%i]: win32 visible [%i] iconic [%i] caption [%i] zoomed [%i] rect [%li %li %li %li]", toggle,
            IsWindowVisible(hwnd) != 0, IsIconic(hwnd) != 0, (style & WS_CAPTION) == WS_CAPTION, IsZoomed(hwnd) != 0,
            rect.left, rect.top, rect.right, rect.bottom);
        os_ok = (background || IsWindowVisible(hwnd)) && !IsIconic(hwnd) && (style & WS_CAPTION) == WS_CAPTION &&
            (!app_maximized || background || IsZoomed(hwnd));
        zox_tst_check(os_ok, "windowed [%i], win32 says it isn't usable (visible, iconic or no caption)", toggle);
    }
#endif
    return !fullscreen && !minimized && !hidden && !borderless && on_display && os_ok && sized;
}

// sdl_window_restore: the same, from a window that isn't maximized (back to its restore size and position)
void zox_tst_sdl_window_restore_setup() {
    maximized = 0;
}

byte zox_tst_sdl_window_mode_ready(ecs* world) {
    if (!zox_tst_sdl_app_ready(world)) {
        return 0;
    }
    zox_tst_progress(zox_tst_wm_stage * 1000 + zox_tst_wm_frames_in);
    zox_tst_waiting = "the window to change mode";
    SDL_Window* window = zox_getv(main_app, SDLWindow);
    // until the window is in the mode asked for (fullscreen or not), then a few frames to settle
    SDL_SyncWindow(window);
    const byte wanted = zox_tst_wm_stage % 2;
    const byte in_mode = ((SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) != 0) == wanted;
    if (zox_tst_wm_frames_in++ < (in_mode ? zox_tst_wm_settle : zox_tst_wm_frames)) {
        return 0;
    }
    // a windowed stage done: check it before the next toggle
    if (zox_tst_wm_stage > 0 && zox_tst_wm_stage % 2 == 0) {
        if (!zox_tst_wm_windowed_ok(world, window, zox_tst_wm_stage / 2) && zox_tst_wm_fails_at < 0) {
            zox_tst_wm_fails_at = zox_tst_wm_stage / 2;
        }
    }
    if (zox_tst_wm_stage >= zox_tst_wm_toggles) {
        return 1;
    }
    zox_tst_wm_stage++;
    zox_tst_wm_frames_in = 0;
    const byte fullscreen = zox_tst_wm_stage % 2;
    zox_log("[tst] toggle [%i]: fullscreen [%i]", zox_tst_wm_stage, fullscreen);
    zox_set_app_fullscreen(world, main_app, fullscreen);
    return 0;
}

void zox_tst_sdl_window_mode_check(ecs* world) {
    (void) world;
    zox_log("[tst] first bad windowed toggle [%i] (-1 none)", zox_tst_wm_fails_at);
}
