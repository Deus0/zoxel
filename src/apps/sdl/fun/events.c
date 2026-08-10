// todo: link sdl app events to sub systems
extern void opengl_dispose_resources(ecs *world);
extern void opengl_restore_resources(ecs *world);
extern void engine_end(); // engine

// move this to an app system function?
/*void update_sdl(ecs *world) {
    SDL_PumpEvents();
    entity e = main_app;
    SDL_Event event = { 0 };
    while (SDL_PollEvent(&event)) {
        // Quit the window
        if (event.type == SDL_QUIT) {
            engine_end();
        }
        // Window Events
        else if (event.type == SDL_WINDOWEVENT) {
            // Window is resized
            if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
            ) {
                int2 window_size = (int2) { event.window.data1, event.window.data2 };
                byte monitor = event.display.display;

                byte orientation = get_screen_orientation(monitor);
                // zox_logv("Display [%i] Orientation [%i] Size Changed [%ix%i]", monitor, orientation, window_size.x, window_size.y);
                zox_geter_value(e, ScreenOrientation, byte, old_orientation);
                if (old_orientation != orientation) {
                    // remove flip, didnt detect
                    zox_set(e, ScreenOrientation, { orientation });
                }
                // zox_log("SDL WINDOW SIZE %ix%i", window_size.x, window_size.y);
                // Re-query drawable size after fullscreen
                // int draw_w, draw_h;
                // SDL_GL_GetDrawableSize(zox_gett_value(e, SDLWindow), &draw_w, &draw_h);
                // zox_log("Fullscreen drawable size %ix%i", draw_w, draw_h);
                on_window_resized(world, e, window_size);
            }
            else if (event.type == SDL_DISPLAYEVENT && event.display.event == SDL_DISPLAYEVENT_ORIENTATION) {
                byte monitor = event.display.display;
                byte orientation = get_screen_orientation(monitor);
                zox_logw("Display [%i] Orientation Changed: %i", monitor, orientation);
                // on_window_rotated(world, e, orientation);
            }
            else if (event.window.event == SDL_WINDOWEVENT_MOVED) { // handles application resizing
                sdl_on_window_moved(world, e, (int2) { event.window.data1, event.window.data2 });
            }
            else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
                on_window_maximized(world, e, (int2) { event.window.data1, event.window.data2 });
            }
            else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
                zox_logv("App Gained Focus + Restored");
                opengl_restore_resources(world);
                enable_time();
                on_window_restored(world, e, (int2) { event.window.data1, event.window.data2 });
            }
            else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                zox_logv("App Lost Focus + Minimized");
                opengl_dispose_resources(world);
                disable_time();
            }
            else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                zox_logv("App Lost Focus");
                disable_time();
            }
            else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                zox_logv("App Gained Focus");
                enable_time();
            }
        }
        else if (event.type == SDL_TEXTINPUT) {
            zox_log("SDL Text Input: %s", event.text.text);
        }
        else if (update_sdl_input(world, e, event)) {
            // bam!
        }
    }
}*/

/*byte has_flipped = (orientation <= 1 && old_orientation <= 1) ||
(orientation >= 2 && old_orientation >= 2);
if (has_flipped) {
    window_size = (int2) { window_size.y, window_size.x };
    zox_logv("Display [%i] Orientation has flipped [%i]", monitor, orientation);
}*/
/*zox_geter_value_non_const(e, SDLWindow, SDL_Window*, sdl_window);
int2 window_size = int2_zero;
SDL_GetWindowSize(sdl_window, &window_size.x, &window_size.y);
on_window_resized(world, e, window_size);*/

void update_sdl(ecs *world) {
    SDL_PumpEvents();
    entity e = main_app;
    SDL_Event event = { 0 };
    while (SDL_PollEvent(&event)) {
        if (sdl_event_quit(&event)) {
            engine_end();
        }
        else if (sdl_event_window_resized(&event)) {
            int2 window_size = sdl_event_window_size(&event);
            byte monitor = sdl_event_display(&event);
            byte orientation = get_screen_orientation(monitor);

            zox_geter_value(e, ScreenOrientation, byte, old_orientation);

            if (old_orientation != orientation) {
                zox_set(e, ScreenOrientation, { orientation });
            }

            on_window_resized(world, e, window_size);
        }
        else if (sdl_event_display_orientation(&event)) {
            byte monitor = sdl_event_display(&event);
            byte orientation = get_screen_orientation(monitor);
            zox_logw(
                "Display [%i] Orientation Changed: %i",
                monitor,
                orientation
            );
        }
        else if (sdl_event_window_moved(&event)) {
            sdl_on_window_moved(
                world,
                e,
                sdl_event_window_size(&event)
            );
        }
        else if (sdl_event_window_maximized(&event)) {
            on_window_maximized(
                world,
                e,
                sdl_event_window_size(&event)
            );
        }
        else if (sdl_event_window_restored(&event)) {
            zox_logv("App Gained Focus + Restored");
            opengl_restore_resources(world);
            enable_time();

            on_window_restored(
                world,
                e,
                sdl_event_window_size(&event)
            );
        }
        else if (sdl_event_window_minimized(&event)) {
            zox_logv("App Lost Focus + Minimized");
            opengl_dispose_resources(world);
            disable_time();
        }
        else if (sdl_event_window_focus_lost(&event)) {
            zox_logv("App Lost Focus");
            disable_time();
        }
        else if (sdl_event_window_focus_gained(&event)) {
            zox_logv("App Gained Focus");
            enable_time();
        }
        else if (sdl_event_text_input(&event)) {
            zox_log("SDL Text Input: %s", sdl_event_text(&event));
        }
        // else if (update_sdl_input(world, e, event)) { }
    }
}