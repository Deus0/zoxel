zox_sys2(MouseConstrainSystem) {
    if (disable_mouse_lock || zox_dbg_touch_with_mouse) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceDisabled);
    zox_sys_in(MouseLock);
    zox_sys_in(AppLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceDisabled, disabled);
        zox_sys_i(MouseLock, lock);
        zox_sys_i(AppLink, app);
        if (!zox_valid(app->value) || disabled->value) {
            continue;
        }
        zox_geter_value_non_const(app->value, SDLWindow, SDL_Window*, sdl_window);
        int2 lock_position = get_mouse_center_point(world, app->value);
        SDL_SetRelativeMouseMode(lock->value);
        if (!lock->value) {
            continue;
        }
        SDL_WarpMouseInWindow(sdl_window, lock_position.x, lock_position.y);
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(world, e, children, children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            if (zox_has(e2, ZevicePointerPosition)) {
                zox_muter(e2, ZevicePointerPosition, position);
                position->value = lock_position;
            }
        }
    }
} zox_sys_end(MouseConstrainSystem);
