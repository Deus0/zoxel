// wrapper for SDL GL Swap Window
void app_update_gpu(ecs_world_t *world) {
    if (headless) {
        return;
    }
    if (!zox_valid(main_app)) {
        zox_log_error("[main_app] is missing");
        return;
    }
    zox_geter(main_app, SDLWindow, window)
    if (!window->value) {
        zox_log_error("[main_app] is has invalid SDLWindow");
        return;
    }
    SDL_GL_SwapWindow(window->value);
}

byte create_window_opengl_context(ecs *world, const entity e) {

    zox_geter(e, SDLWindow, sdl_window);
    SDL_GLContext context = create_sdl_opengl_context(sdl_window->value);
    if (!context) {
        zox_log_error("OpenGL could not create a context");
        running = 0;
        return EXIT_FAILURE;
    }

    if (set_sdl_window_context(sdl_window->value, context)) {
        zox_log_error("OpenGL could not set context to sdl window");
        running = 0;
        return EXIT_FAILURE;
    }

    // --- Now check if the context is ACTUALLY valid ---
    const char* ver = (const char*)glGetString(GL_VERSION);
    const char* ren = (const char*)glGetString(GL_RENDERER);
    const char* ven = (const char*)glGetString(GL_VENDOR);

    if (!ver || !ren || !ven) {
        zox_log_error("OpenGL context creation failed: GL strings are NULL.");
        SDL_GL_DeleteContext(context);
        return EXIT_FAILURE;
    }

    zox_log("GL_VERSION: %s", ver);
    zox_log("GL_RENDERER: %s", ren);
    zox_log("GL_VENDOR: %s", ven);

    zox_log("Created Opengl Context Success");
    zox_set(e, Context, { context });

    return EXIT_SUCCESS;
}
