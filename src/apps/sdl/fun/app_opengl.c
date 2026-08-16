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
    if (!sdl_gl_make_current(sdl_window->value, context)) {
        zox_loge("Failed to make OpenGL context current: %s", SDL_GetError());
        running = 0;
        return 0;
    }
    zox_setv(e, Context, context);
    zox_log("OpenGL Context Created");
    zox_log("   GL_VERSION: %s", glGetString(GL_VERSION));
    zox_log("   GL_RENDERER: %s", glGetString(GL_RENDERER));
    zox_log("   GL_VENDOR: %s", glGetString(GL_VENDOR));
    if (!is_verbose) {
        return e;
    }
#ifdef zox_verbose
    zox_logv("OpenGL Context Created");
    zox_logv("   GL_VERSION: %s", glGetString(GL_VERSION));
    zox_logv("   GL_RENDERER: %s", glGetString(GL_RENDERER));
    zox_logv("   GL_VENDOR: %s", glGetString(GL_VENDOR));
#endif
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
    set_sdl_app_settings(name);
    // Window creates and binds OpenGL Context too!
    zox_logv("Spawning SDL Window [%s]", name);
    entity app = spawn_app_sdl_opengl(world, name, fullscreen, maximized, monitor);
    if (!app) {
        zox_loge("[engine_spawn_window] failed");
        return 0;
    }
    // Link to a global
    main_app = app;
    return app;
}
