zox_sys2(MouseConstrainSystem) {
    if (disable_mouse_lock) {
        return;
    }
    zox_sys_world();
    // todo: support for multi window contstraints, pass in multiple sdl windows here
    //if (!zox_valid(main_app)) {
    //    return;
    //}
    //zox_geter_value_non_const(main_app, SDLWindow, SDL_Window*, sdl_window)
    zox_sys_begin();
    zox_sys_in(DeviceDisabled);
    zox_sys_in(MouseLock);
    zox_sys_in(Children);
    zox_sys_in(AppLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(DeviceDisabled, disabled);
        zox_sys_i(MouseLock, lock);
        zox_sys_i(Children, children);
        zox_sys_i(AppLink, app);

        if (!zox_valid(app->value) || disabled->value) {
            continue;
        }

        zox_geter_value_non_const(app->value, SDLWindow, SDL_Window*, sdl_window);

        int2 lock_position = get_mouse_center_point(world, app->value);
        // now lock
        SDL_SetRelativeMouseMode(lock->value);
        if (lock->value) {
            SDL_WarpMouseInWindow(sdl_window, lock_position.x, lock_position.y);
            for (int j = 0; j < children->length; j++) {
                const entity zevice = children->value[j];
                if (zox_has(zevice, ZevicePointerPosition)) {
                    zox_muter(zevice, ZevicePointerPosition, position)
                    position->value = lock_position;
                }
            }
        }
    }
} zox_sys_end(MouseConstrainSystem);
