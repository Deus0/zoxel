zox_sys2(MouseConstrainSystem) {
    byte dbg_log = 0;
    if (disable_mouse_lock || zox_dbg_touch_with_mouse) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MouseLock);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MouseLock, lock);
        entity app = zox_get_link(world, e, AppLink);
        if (!zox_valid(app) || !zox_has(app, SDLWindow)) {
            continue;
        }
        const SDLWindow* window = zox_get(app, SDLWindow);
        int2 lock_position = get_mouse_center_point(
            world,
            app);
        zox_sdl_set_relative_mouse_mode(
            window->value,
            lock->value);
        if (!lock->value) {
            continue;
        }
        sdl_warp_mouse_in_window(
            window->value,
            lock_position.x,
            lock_position.y);
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(
            world,
            e,
            children,
            children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            if (zox_has(e2, ZevicePointerPosition)) {
                zox_muter(e2, ZevicePointerPosition, position);
                position->value = lock_position;
            }
        }
        if (dbg_log) {
            zox_log("lock_position [%ix%i]",
                lock_position.x,
                lock_position.y);
        }
    }
} zox_sys_end(MouseConstrainSystem);
