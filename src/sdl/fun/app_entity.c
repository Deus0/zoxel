void zox_set_app_fullscreen(
    ecs* world,
    entity e,
    byte fullscreen
) {
    if (!zox_valid(e) || !zox_has(e, WindowFullscreen)) {
        zox_log_error("invalid app in [zox_set_app_fullscreen]")
        return;
    }
    zox_set(e, WindowFullscreen, { fullscreen });
    zox_app_set_fullscreen(
        zox_gett_value(e, SDLWindow),
        fullscreen);
    int2 size;
    if (fullscreen) {
        size = get_screen_size();
    } else {
        size = zox_get_value(e, WindowSizeRestore)
        /*int2 screen_size = get_screen_size();
        if (int2_equals(screen_size, size)) {
            size = int2_half(screen_size);
            zox_log("> window size was set to half screen size")
        }*/
    }
    if (!int2_equals(size, zox_gett_value(e, WindowSize))) {
        zox_set(e, WindowSize, { size });
        zox_set(e, WindowSizeDirty, { zox_dirty_trigger });
    }
    zox_log_sdl("+ setting app [%s]", fullscreen ? "fullscreen" : "windowed");
}

// sdl implementation for maximized state
void zox_set_app_maximized(ecs* world, entity e, byte maximized) {
    if (!zox_valid(e) || !zox_has(e, WindowFullscreen)) {
        zox_log_error("invalid app in [zox_set_app_fullscreen]")
        return;
    }
    zox_geter_value(e, WindowFullscreen, byte, fullscreen);
    zox_set(e, WindowMaximized, { maximized })
    if (!fullscreen) {
        int2 size;
        int2 position;
        if (maximized) {
            size = get_maximized_size(world, e);
            // position = int2_half(get_screen_size());
            zox_log_sdl("+ maximizing app [%ix%i]", size.x, size.y)
            zox_app_set_size(world, e, size);
        } else {
            zox_geter_value(e, WindowSizeRestore, int2, restore_size);
            zox_geter_value(e, WindowPositionRestore, int2, restore_position);
            size = restore_size;
            position = restore_position;
            zox_log_sdl("+ restoring app [%ix%i] at [%ix%i]", size.x, size.y, position.x, position.y);
            zox_app_set_size(world, e, size);
            zox_app_set_position(world, e, position);
        }
    }
}


// todo: get position2 to work
entity spawn_app_sdl_opengl(
    ecs *world,
    const char* name,
    const byte fullscreen,
    const byte maximized,
    const byte monitor
) {
    const entity e = spawn_app_sdl(
        world,
        name,
        fullscreen,
        maximized,
        monitor
    );
    if (!e) {
        zox_log_error("failed spawning sdl window");
        return 0;
    }
    if (create_window_opengl_context(world, e) == EXIT_FAILURE) {
        zox_log_error(" opengl_context creation failed");
        return 0;
    }

    /*zox_geter(e, SDLWindow, window);
    SDL_GL_SwapWindow(window->value);
    if (SDL_GL_SetSwapInterval(vsync)) {
        zox_log_error("Unable to disable VSync: %s", SDL_GetError());
    }*/

    return e;
}


extern byte load_app_icon(SDL_Window*, const char*);

byte spawn_window_icon(ecs *world, const entity app, const char* texture_name) {
    if (!app) {
        zox_log_error("app not spawned");
        return 1;
    }
    // Shaders
    char* path_textures = concat_file_path(resources_path, "textures");  //  character_slash filename
    if (!path_textures) {
        return 1;
    }
    char* path_icon = concat_file_path(path_textures, texture_name);  //  character_slash filename
    free(path_textures);
    if (!path_icon) {
        return 1;
    }
    load_app_icon(zox_gett_value(app, SDLWindow), path_icon);
    free(path_icon);
    return 0;
}
