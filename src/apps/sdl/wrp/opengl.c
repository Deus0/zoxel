// Probes core Compatibility, assumes SDL is initialized
byte supports_opengl_version(byte major, byte minor, byte version) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, version);
    SDL_Window* window = zox_sdl_create_window("probe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 16, 16, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!window) {
        return 0;
    }
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        SDL_DestroyWindow(window);
        return 0;
    }
    zox_sdl_gl_delete_context(context);
    SDL_DestroyWindow(window);
    return 1;
}


byte supports_opengl_core(byte major, byte minor) {
    return supports_opengl_version(major, minor, SDL_GL_CONTEXT_PROFILE_CORE);
}

void set_sdl_attributes2(byte minor, byte major, byte profile) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, sdl_gl_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, sdl_gl_minor);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // 24 | 32
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile);
    zox_logv("  - OpenGL p[%i] v[%i.%i]", profile, sdl_gl_major, sdl_gl_minor);
}

void set_sdl_attributes() {
    if (opengl_mode == zox_opengl_core) {
        if (!supports_opengl_core(sdl_gl_major, sdl_gl_minor)) {
            zox_logw("OpenGL Core Not Supported");
            opengl_mode = zox_opengl_es; // zox_opengl_compatibility;
        }
    }
    if (opengl_mode == zox_opengl_es) {
        if (!supports_opengl_version(sdl_gl_major, sdl_gl_minor, SDL_GL_CONTEXT_PROFILE_ES)) {
            zox_logw("OpenGL ES Not Supported");
            opengl_mode = zox_opengl_compatibility;
        }
    }
    zox_logv("Graphics Backend [%s]",
             opengl_mode == zox_opengl_es ? "OpenGL ES" :
             (opengl_mode == zox_opengl_core ? "OpenGL Core" : "OpenGL Compatibility"));
    byte profile = opengl_mode == zox_opengl_es ? SDL_GL_CONTEXT_PROFILE_ES :
             (opengl_mode == zox_opengl_core ? SDL_GL_CONTEXT_PROFILE_CORE : SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    set_sdl_attributes2(sdl_gl_major, sdl_gl_minor, profile);
}

SDL_GLContext create_sdl_opengl_context(SDL_Window* window) {
    if (!window) {
        return NULL;
    }
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        zox_log_error("[create_sdl_opengl_context:SDL_GL_CreateContext] [%s]", SDL_GetError());
        return NULL;
    }
    return context;
}

byte set_sdl_window_context(SDL_Window* window, SDL_GLContext context) {
    if (!window || !context) {
        zox_loge("Window or Context is null.");
        return 1;
    }
    if (!sdl_gl_make_current(window, context)) {
        zox_loge("Failed to make OpenGL context current: %s\n", SDL_GetError());
        return 1;
    }
    // SDL_GL_SetSwapInterval(vsync);
    return 0;
}
