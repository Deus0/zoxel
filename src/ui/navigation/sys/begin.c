zox_sys2(ElementNavigationBeginSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceMode);
    zox_sys_in(CanvasLink);
    zox_sys_out(RaycasterTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceMode, dmode);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_o(RaycasterTarget, current);

        if (zox_valid(current->value)) {
            continue;
        }

        byte device_mode = dmode->value;
#ifdef zox_debug_navigation
        device_mode = zox_device_mode_gamepad;
#endif

        if (device_mode != zox_device_mode_gamepad) {
            continue;
        }

        find_child_with_tag(canvas->value, Window, window);
        if (window) {
            raycaster_select_first_button(world, e, window);
        }

    }
} zox_sys_end(ElementNavigationBeginSystem);