// todo: link sdl app events to sub systems
extern void opengl_dispose_resources(ecs *world);
extern void opengl_restore_resources(ecs *world);
extern void engine_end(); // engine

// move this to an app system function?
void update_sdl(ecs *world) {
    const entity e = main_app;
    input_reset_sdl();
    SDL_Event event = { 0 };
    while (SDL_PollEvent(&event)) {
        input_extract_from_sdl(world, event);
        if (event.type == SDL_QUIT) {
            engine_end();
        } else if (event.type == SDL_WINDOWEVENT) {
            // Window is resized
            if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
            ) {
                byte monitor = event.display.display;
                byte orientation = get_screen_orientation(monitor);
                int2 window_size = (int2) { event.window.data1, event.window.data2 };
                zox_logv("Display [%i] Orientation [%i] Size Changed [%ix%i]", monitor, orientation, window_size.x, window_size.y);

                zox_geter_value(e, ScreenOrientation, byte, old_orientation);
                if (old_orientation != orientation) {
                    // remove flip, didnt detect
                    /*byte has_flipped = (orientation <= 1 && old_orientation <= 1) ||
                    (orientation >= 2 && old_orientation >= 2);
                    if (has_flipped) {
                        window_size = (int2) { window_size.y, window_size.x };
                        zox_logv("Display [%i] Orientation has flipped [%i]", monitor, orientation);
                    }*/
                    zox_set(e, ScreenOrientation, { orientation });
                }
                on_window_resized(world, e, window_size);
            }

            else if (event.type == SDL_DISPLAYEVENT && event.display.event == SDL_DISPLAYEVENT_ORIENTATION) {
                byte monitor = event.display.display;
                byte orientation = get_screen_orientation(monitor);
                zox_logw("Display [%i] Orientation Changed: %i", monitor, orientation);
                // on_window_rotated(world, e, orientation);

                /*zox_geter_value_non_const(e, SDLWindow, SDL_Window*, sdl_window);
                int2 window_size = int2_zero;
                SDL_GetWindowSize(sdl_window, &window_size.x, &window_size.y);
                on_window_resized(world, e, window_size);*/
            }

            else if (event.window.event == SDL_WINDOWEVENT_MOVED) { // handles application resizing
                sdl_on_window_moved(world, e, (int2) { event.window.data1, event.window.data2 });
            } else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
                on_window_maximized(world, e, (int2) { event.window.data1, event.window.data2 });
            } else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
                opengl_restore_resources(world);
                enable_time();
                on_window_restored(world, e, (int2) { event.window.data1, event.window.data2 });
            } else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                opengl_dispose_resources(world);
                disable_time();
            } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                disable_time();
            } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                enable_time();
            } /*else {
                zox_log("-> unknown sdl event [%i]", event.window.event)
            }*/
        }
    }
}
