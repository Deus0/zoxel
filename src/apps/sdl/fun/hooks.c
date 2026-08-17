void sdl_on_window_moved(ecs *world, entity e, int2 position) {
    zox_logv("> sdl_on_window_moved [%ix%i]", position.x, position.y);
    zox_set(e, WindowPosition, { position });
    if (!zox_getv(e, WindowFullscreen) && !zox_getv(e, WindowMaximized)) {
        zox_set(e, WindowPositionRestore, { position });
        zox_logv("+ setting window restore position to [%ix%i]", position.x, position.y);
    }
    zox_geter_value(e, WindowMonitor, byte, old_monitor_index);
    byte monitor_index = zox_app_get_monitor(world, e);
    if (old_monitor_index != monitor_index) {
        zox_set(e, WindowMonitor, { monitor_index });
        zox_log("TODO: Set monitor setting here");
        // zox_set_monitor_silently(world, e, monitor_index);
        if (is_log_monitors) {
            zox_log("+ window moved to monitor [%i]", monitor_index);
        }
    }
}

void on_window_resized(ecs *world, entity e, int2 size, byte dbg_log) {
    // Cache the size before we resize
    byte fullscreen = zox_getv(e, WindowFullscreen);
    if (!fullscreen && !zox_getv(e, WindowMaximized)) {
        zox_set(e, WindowSizeRestore, { size });
        // zox_log("WindowSizeRestore Set [%ix%i]", size.x, size.y);
    }
    /*if (fullscreen  && override_system_scaling) {
        zox_log("Overriding shitty SDL Size from [%ix%i]", size.x, size.y);
        zox_geter_value(e, WindowMonitor, byte, monitor);
        size = get_screen_size_monitor(monitor);
        zox_log("Overrid shitty SDL Size to [%ix%i] on monitor [%i]", size.x, size.y, monitor);
    }*/
    // Only work if resizing our entity
    if (int2_equals(size, zox_getv(e, WindowSize))) {
        return;
    }
    // resize
    zox_set(e, WindowSize, { size });
    zox_set(e, WindowSizeDirty, { zox_dirty_trigger });
    if (dbg_log) {
        zox_logv("+ WindowSizeDirty [%ix%i]", size.x, size.y);
    }
}

/*void on_window_rotated(ecs* world, const entity e, byte orientation) {
    zox_geter_value(e, ScreenOrientation, byte, old_orientation);
    if (orientation == old_orientation) {
        return;
    }
    if ((orientation <= 1 && old_orientation <= 1) ||
        (orientation >= && old_orientation  >= 2)) {
        zox_log("Orientation ratio still the same.");
        return;
    }
    // change window dimensions
}*/

