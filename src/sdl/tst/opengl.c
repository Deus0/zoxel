// Probes core Compatibility, assumes SDL is initialized

byte supports_opengl_version(byte major, byte minor, byte version) {

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, version);

    SDL_Window* window = SDL_CreateWindow("probe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 16, 16, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!window) {
        return 0;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        SDL_DestroyWindow(window);
        return 0;
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    return 1;
}

byte supports_opengl_core(byte major, byte minor) {
    return supports_opengl_version(major, minor, SDL_GL_CONTEXT_PROFILE_CORE);
}
