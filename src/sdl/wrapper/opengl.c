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

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // 24 | 32
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, sdl_gl_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, sdl_gl_minor);
    if (is_log_sdl) {
        zox_logi("OpenGL Version [%i.%i]", sdl_gl_major, sdl_gl_minor);
    }

    if (opengl_mode == zox_opengl_es) {
        zox_logi("Running with OpenGL ES");
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    } else if (opengl_mode == zox_opengl_core) {
        zox_logi("Running with OpenGL Core");
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    } else {
        zox_logi("Running with OpenGL Compatibility");
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    }
}


byte set_sdl_window_context(SDL_Window* window, SDL_GLContext context) {
    if (!window || !context) {
        return 1;
    }
    if (SDL_GL_MakeCurrent(window, context)) {
        zox_log_error("Failed to make OpenGL context current: %s\n", SDL_GetError());
        return 1;
    }
    return 0;
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
