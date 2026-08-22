entity spawn_prefab_app_sdl(ecs *world) {
    zox_prefab_child(prefab_app);
    zox_prefab_name("app_sdl");
    zox_prefab_set(e, SDLWindow, { NULL });
    zox_prefab_set(e, Context, { NULL });
    zox_prefab_set(e, WindowFullscreen, { 0 });
    // zox_add(e, ViewPort)
    return e;
}

entity spawn_app_sdl(
    ecs *world,
    const char* name,
    byte fullscreen,
    byte maximized,
    byte monitor)
{
    byte max_monitors = zox_sdl_get_num_displays();
    if (monitor >= max_monitors) {
        zox_logw("Monitor [%i] was over max [%i]", monitor, max_monitors);
        monitor = 0;
    }
    byte screen_orientation = get_screen_orientation(monitor);
    int2 screen_size = get_screen_size_monitor(monitor);
    int2 size_restore = int2_scale1(screen_size, 0.6f);
    size_restore = int2_single(int_min(size_restore.x, size_restore.y));
    int2 size = fullscreen ? screen_size : size_restore;
    // calculate position
    int2 position = calculate_monitor_position(monitor, 1, size_restore);
    SDL_WindowFlags flags = zox_sdl_window_flags(fullscreen, maximized);
    SDL_Window* sdl_window = create_sdl_window(position, size, name, flags);
    if (!sdl_window) {
        zox_loge(" opengl did not create sdl_window, exiting zoxel")
        return 0;
    }
    if (!fullscreen && maximized) {
        zox_app_set_maximized(sdl_window, maximized);
    }
    zox_instance(prefab_app_sdl);
    zox_name("app_sdl");
    set_ZoxName(world, e, name);
    zox_setv(e, SDLWindow, sdl_window);
    // current transform data
    zox_setv(e, WindowPosition, position);
    zox_setv(e, WindowSize, size);
    // window restore data
    zox_setv(e, WindowPositionRestore, position);
    zox_setv(e, WindowSizeRestore, size_restore);
    // our properties
    zox_setv(e, WindowFullscreen, fullscreen);
    zox_setv(e, WindowMaximized, maximized);
    zox_setv(e, WindowMonitor, monitor);
    zox_setv(e, ScreenOrientation, screen_orientation);
    // debugs
    zox_logv("+ spawned window !");
    zox_logv("   - position [%ix%i]", position.x, position.y);
    zox_logv("   - size [%ix%i]", size.x, size.y);
    zox_logv("   - fullscreen [%i]", fullscreen);
    zox_logv("   - maximized [%i]", maximized);
    zox_logv("   - monitor [%i]", monitor);
    zox_logv("   - screen orientation [%i]", screen_orientation);
    return e;
}
