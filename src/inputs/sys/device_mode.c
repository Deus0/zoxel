// NOTE: System responsible for device switching for a DeviceUser
// TODO: Grab not used devices from DeviceManager instead of local ones
// TODO: It goes through devices 3 times... thats bad!
zox_sys2(DeviceSwitchSystem) {
    byte dbg_log = 0;
    if (!auto_switch_device) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(DeviceModeDirty);
    zox_sys_out(DeviceMode);
    zox_sys_out(LastDeviceMode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(DeviceModeDirty, dirty);
        zox_sys_o(DeviceMode, mode);
        zox_sys_o(LastDeviceMode, last);
        if (dirty->value == zox_dirty_active) {
            dirty->value = 0;
            continue;
        }
        if (dirty->value != 0) { // zox_dirty_trigger) {
            continue;
        }
        // first check if currently using selected inputs
        byte using_current = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2) && !using_current) {
            for (int j = 0; j < it2.count && !using_current; j++) {
                entity e2 = it2.entities[j];
                if (!zox_has(e2, Device)) {
                    continue;
                }
                if (mode->value == zox_device_mode_keyboardmouse) {
                    if (zox_has(e2, Keyboard)) {
                        zox_geter(e2, Keyboard, keyboard);
                        if (keyboard_is_any_input(keyboard)) {
                            using_current = 1;
                        }
                    } else if (zox_has(e2, Mouse)) {
                        if (mouse_is_any_input(world, e2)) {
                            using_current = 1;
                        }
                    }
                } else if (mode->value == zox_device_mode_gamepad) {
                    if (zox_has(e2, Gamepad) && zox_getv(e2, DeviceHasInput)) {
                        using_current = 1;
                    }
                } else if (mode->value == zox_device_mode_touchscreen) {
                    if (zox_has(e2, Touchscreen)) {
                        if (touchscreen_is_any_input(world, e2)) {
                            using_current = 1;
                        }
                    }
                }
            }
        }
        if (using_current) {
            if (dbg_log >= 2) {
                zox_log("Using Current Device [%i]",
                    mode->value);
            }
            continue;
        }
        if (dbg_log >= 2) {
            uint devices_length = zox_get_children_count_by_id(
                world,
                e,
                zox_id(Device));
            zox_log("Devices %i; Mode [%i]",
                devices_length,
                mode->value);
        }
        byte new_mode = 0;
        it2 = zox_children(world, e);
        while (zox_children_next(it2) && !new_mode) {
            for (int j = 0; j < it2.count && !new_mode; j++) {
                entity e2 = it2.entities[j];
                if (dbg_log >= 2) {
                    zox_log("   Device %i: %s", j, zox_get_name(e2));
                }
                if (mode->value != zox_device_mode_keyboardmouse) {
                    if (zox_has(e2, Keyboard)) {
                        zox_geter(e2, Keyboard, keyboard);
                        if (keyboard_is_any_input(keyboard)) {
                            new_mode = zox_device_mode_keyboardmouse;
                        }
                    } else if (zox_has(e2, Mouse)) {
                        if (mouse_is_any_input(world, e2)) {
                            new_mode = zox_device_mode_keyboardmouse;
                        }
                    }
                }
                if (mode->value != zox_device_mode_gamepad) {
                    if (zox_has(e2, Gamepad) && zox_getv(e2, DeviceHasInput)) {
                        new_mode = zox_device_mode_gamepad;
                    }
                }
                if (mode->value != zox_device_mode_touchscreen) {
                    if (zox_has(e2, Touchscreen)) {
                        if (touchscreen_is_any_input(world, e2)) {
                            new_mode = zox_device_mode_touchscreen;
                        }
                    }
                }
            }
        }
        if (!new_mode) {
            continue;
        }
        // set player links here if dirty
        it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (new_mode == zox_device_mode_keyboardmouse) {
                    byte enabled =
                        zox_has(e2, Keyboard) ||
                        zox_has(e2, Mouse);
                    zox_set_tag(e2, Disabled, !enabled);
                } else if (new_mode == zox_device_mode_gamepad) {
                    byte enabled = zox_has(e2, Gamepad);
                    zox_set_tag(e2, Disabled, !enabled);
                } else if (new_mode == zox_device_mode_touchscreen) {
                    byte enabled = zox_has(e2, Touchscreen);
                    zox_set_tag(e2, Disabled, !enabled);
                }
            }
        }
        last->value = mode->value;
        mode->value = new_mode;
        dirty->value = zox_dirty_active;
        disable_inputs_until_release(world, e, mode->value);
        if (dbg_log) {
            // zox_logv(" + device mode switched to [%i] from [%i]", dirty->value, deviceMode->value)
            if (new_mode== zox_device_mode_keyboardmouse) {
                zox_log("Device mode changed to [keyboardmouse]")
            } else if (new_mode == zox_device_mode_touchscreen) {
                zox_log("Device mode changed to [touchscreen]")
            } else if (new_mode == zox_device_mode_gamepad) {
                zox_log("Device mode changed to [gamepad]")
            }
        }
    }
} zox_sys_end(DeviceSwitchSystem);
