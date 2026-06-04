zox_sys2(QolShortcutsSystem) {
    byte toggle_fullscreen = 0;
    byte toggle_maximized = 0;
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!zox_valid(e2) || !zox_has(e2, Keyboard)) {
                continue;
            }
            zox_geter(e2, Keyboard, keyboard);
            if (keyboard->f11.pressed_this_frame) {
                toggle_fullscreen = 1;
            } else if (keyboard->f10.pressed_this_frame) {
                toggle_maximized = 1;
            } else if (keyboard->f8.pressed_this_frame) {
                is_render_fog = !is_render_fog;
            }
        }
    }
    if (toggle_fullscreen) {
        zox_toggle_fullscreen(world);
    }
    if (toggle_maximized) {
        zox_toggle_maximized(world);
    }
} zox_sys_end(QolShortcutsSystem);
