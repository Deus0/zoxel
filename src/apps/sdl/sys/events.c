// todo: link sdl app events to sub systems
extern void opengl_dispose_resources(ecs *world);
extern void opengl_restore_resources(ecs *world);
extern void engine_end(); // engine
//  TODO: Use event's window to fetch the entity properly

void sdl_events_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    reset_sdl_inputs(world);
    entity e = main_app;
    if (!zox_valid(e) || !zox_has(e, SDLWindow)) {
        return;
    }
    const SDLWindow* sdl_window = zox_get(e, SDLWindow);
    SDL_PumpEvents();
    SDL_Event event = { 0 };
    while (SDL_PollEvent(&event)) {
        if (sdl_event_quit(&event)) {
            engine_end();
        } else if (sdl_event_window_resized(&event)) {
            sbyte monitor = sdl_event_get_monitor(&event);
            if (monitor < 0) {
                // NOTE: Likely transient / unowned window
                // zox_logw("[sdl_event_window_resized] Bad monitor: [%u] windowID [%u]", event.window.windowID, event.type);
                continue;
            }
            byte orientation = get_screen_orientation(monitor);
            int2 window_size = sdl_event_window_size(&event);
            byte old_orientation = zox_getv(e, ScreenOrientation);
            if (old_orientation != orientation) {
                zox_setv(e, ScreenOrientation, orientation);
                if (dbg_log) {
                    zox_log("Screen Orientation Set [%i]", orientation);
                }
            }
            // NOTE: On orientation it will just change the window dimensions
            on_window_resized(
                world,
                e,
                window_size,
                dbg_log);
            if (dbg_log) {
                zox_log("Window Resized to [%ix%i]", window_size.x, window_size.y);
            }
        } else if (sdl_event_display_orientation(&event)) {
            sbyte monitor = sdl_event_get_monitor(&event);
            if (monitor < 0) {
                zox_loge("[sdl_event_display_orientation] Bad monitor");
                continue;
            }
            byte orientation = get_screen_orientation(monitor);
            if (dbg_log) {
                zox_log(
                    "Display [%i] Orientation Changed: %i",
                    monitor,
                    orientation
                );
            }
            // on_window_rotated(world, e, orientation);
        } else if (sdl_event_window_moved(&event)) {
            sdl_on_window_moved(
                world,
                e,
                sdl_event_window_size(&event)
            );
        } else if (sdl_event_window_maximized(&event)) {
            // TODO: Add to app's settings here
            if (dbg_log) {
                zox_log("App Maximized");
            }
        } else if (sdl_event_text_input(&event)) {
            if (dbg_log) {
                zox_log("SDL Text Input: %s", sdl_event_text(&event));
            }
        } else if (sdl_event_window_focus_gained(&event)) {
            enable_time();
#ifdef zox_android
            opengl_restore_resources(world);
#endif
            if (dbg_log) {
                zox_log("App Gained Focus [%s]",
                zox_getn(e));
            }
        } else if (sdl_event_window_restored(&event) ||
            sdl_event_background_left(&event)
        ) {
            enable_time();
            opengl_restore_resources(world);
            if (dbg_log) {
                zox_log("App Restored [%s]", zox_getn(e));
            }
        } else if (sdl_event_window_minimized(&event) ||
            sdl_event_background_entered(&event)
        ) {
            opengl_dispose_resources(world);
            disable_time();
            if (dbg_log) {
                zox_log("App Frozen [%s]", zox_getn(e));
            }
        } else if (sdl_event_window_focus_lost(&event)) {
            disable_time();
#ifdef zox_android
            opengl_dispose_resources(world);
#endif
            if (sdl_window_is_minimized(sdl_window->value)) {
                opengl_dispose_resources(world);
                if (dbg_log) {
                    zox_log("App Frozen [%s]", zox_getn(e));
                }
            } else {
                if (dbg_log) {
                    zox_log("App Lost Focus [%s]", zox_getn(e));
                }
            }
        }
        // handle input events
        update_sdl_input(world, e, event);
    }
} zoxd_system(sdl_events_system);
