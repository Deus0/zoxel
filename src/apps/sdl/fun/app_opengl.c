// todo: get position2 to work
entity spawn_app_sdl_opengl(ecs *world, const char* name, byte fullscreen, byte maximized, byte monitor) {
    entity e = spawn_app_sdl(world, name, fullscreen, maximized, monitor);
    if (!e) {
        zox_loge("Failed spawning sdl window");
        return 0;
    }
    zox_geter(e, SDLWindow, sdl_window);
    SDL_GLContext context = create_sdl_opengl_context(sdl_window->value);
    if (!context) {
        zox_loge("OpenGL could not create a context");
        running = 0;
        return 0;
    }
    if (set_sdl_window_context(sdl_window->value, context)) {
        zox_loge("OpenGL could not set context to sdl window");
        running = 0;
        return 0;
    }
    // --- Now check if the context is ACTUALLY valid ---
    const char* ver = (const char*)glGetString(GL_VERSION);
    const char* ren = (const char*)glGetString(GL_RENDERER);
    const char* ven = (const char*)glGetString(GL_VENDOR);
    /*if (!ver || !ren || !ven) {
        zox_log_error("OpenGL context creation failed: GL strings are NULL.");
        SDL_GL_DeleteContext(context);
        return EXIT_FAILURE;
    }*/
    zox_logv("OpenGL Context Created");
    zox_logv("   GL_VERSION: %s", ver);
    zox_logv("   GL_RENDERER: %s", ren);
    zox_logv("   GL_VENDOR: %s", ven);
    // zox_log("Created Opengl Context Success");
    zox_set(e, Context, { context });
    return e;
}


/*#ifdef zox_game
extern const char* game_name;
#endif*/

// uses modules: App, SDL, Rendering
entity spawn_engine_app(ecs* world, const char* name) {
    if (headless) {
        zox_logw("Headless should not reach here.");
        return 0;
    }
    // Window creates and binds OpenGL Context too!
    zox_logv("Spawning SDL Window [%s]", name);
    entity app = spawn_app_sdl_opengl(world, name, fullscreen, maximized, monitor);
    if (!app) {
        zox_log_error("[engine_spawn_window] failed");
        return 0;
    }
    // Link to a global
    main_app = app;
    return app;
}
