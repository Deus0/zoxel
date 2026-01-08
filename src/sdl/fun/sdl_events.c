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
                int2 window_size = (int2) { event.window.data1, event.window.data2 };
                zox_logv("Screen Size Changed [%ix%i]", window_size.x, window_size.y);
                on_window_resized(world, e, window_size);
            }

            else if (event.type == SDL_DISPLAYEVENT && event.display.event == SDL_DISPLAYEVENT_ORIENTATION) {
                byte display = event.display.display;
                byte orientation = get_screen_orientation(display);
                zox_logv("Display [%i] Orientation Changed: %i", display, orientation);
                // on_window_rotated(world, e, orientation);

                zox_geter_value_non_const(e, SDLWindow, SDL_Window*, sdl_window);
                int2 window_size = int2_zero;
                SDL_GetWindowSize(sdl_window, &window_size.x, &window_size.y);
                on_window_resized(world, e, window_size);
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
