// tst: sdl_alt_tab, alt tabbing out of a fullscreen window and back doesn't leave the game frozen
//  - fullscreen minimizes on focus loss: it disposes gpu resources, stops rendering and time
//  - wayland may never send restored, so gaining focus again restores it (apps/sdl/sys/events.c)
//  - pushes real sdl window events (minimized, focus lost, then focus gained) through sdl_events_system
//  - checks: minimized, it stops rendering; focus back, it renders and time runs again
//  - run: bash bsh/tst.sh sdl_alt_tab
int zox_tst_sat_stage = 0;
int zox_tst_sat_frames = 0;
byte zox_tst_sat_froze = 0, zox_tst_sat_rendering = 0, zox_tst_sat_time = 0;

void zox_tst_sat_push(ecs* world, Uint32 type) {
    SDL_Window* window = zox_valid(main_app) ? zox_getv(main_app, SDLWindow) : NULL;
    SDL_Event event = { 0 };
    event.type = type;
    event.window.windowID = window ? SDL_GetWindowID(window) : 0;
    SDL_PushEvent(&event);
}

byte zox_tst_sdl_alt_tab_ready(ecs* world) {
    (void) world;
    if (!zox_valid(main_app) || !zox_has(main_app, SDLWindow)) {
        zox_tst_waiting = "the app's window";
        return 0;
    }
    zox_tst_sat_frames++;
    if (zox_tst_sat_stage == 0) {
        // a test's window may start hidden (frozen as minimized): focus in first, it should render
        zox_tst_waiting = "rendering, focused";
        if (zox_tst_sat_frames == 1) {
            zox_tst_sat_push(world, SDL_EVENT_WINDOW_FOCUS_GAINED);
        }
        if (zox_tst_sat_frames < 10 || !rendering) {
            return 0;
        }
        zox_tst_sat_push(world, SDL_EVENT_WINDOW_FOCUS_LOST);
        zox_tst_sat_push(world, SDL_EVENT_WINDOW_MINIMIZED);
        zox_tst_sat_stage = 1;
        zox_tst_sat_frames = 0;
        return 0;
    }
    if (zox_tst_sat_stage == 1) {
        zox_tst_waiting = "the minimize";
        if (zox_tst_sat_frames < 4) {
            return 0;
        }
        zox_tst_sat_froze = !rendering && minimized;
        // back in: only focus, no restored (as wayland)
        zox_tst_sat_push(world, SDL_EVENT_WINDOW_FOCUS_GAINED);
        zox_tst_sat_stage = 2;
        zox_tst_sat_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the focus back";
    if (zox_tst_sat_frames < 10) {
        return 0;
    }
    zox_tst_sat_rendering = rendering && !minimized;
    zox_tst_sat_time = updating_time;
    return 1;
}

void zox_tst_sdl_alt_tab_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_sat_froze, "minimized, it kept rendering (the test's events didn't arrive?)");
    zox_tst_check(zox_tst_sat_rendering, "focus back (no restored event), it's still not rendering: stuck");
    zox_tst_check(zox_tst_sat_time, "focus back, time is still paused: stuck");
}
