// NOTE: System responsible for device switching for a DeviceUser
// TODO: Grab not used devices from DeviceManager instead of local ones
zox_sys2(DeviceSwitchSystem) {
    if (!auto_switch_device) {
        return;
    }
    byte dbg_log = 0;
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
        byte using_current_inputs = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (mode->value == zox_device_mode_keyboardmouse) {
                if (zox_has(e2, Keyboard)) {
                    zox_geter(e2, Keyboard, keyboard)
                    if (keyboard_is_any_input(keyboard)) {
                        // zox_log(" > using current keyboard\n")
                        using_current_inputs = 1;
                        break;
                    }
                } else if (zox_has(e2, Mouse)) {
                    if (mouse_is_any_input(world, e2)) {
                        using_current_inputs = 1;
                        // zox_log(" > using current mouse\n")
                        break;
                    }
                }
            } else if (mode->value == zox_device_mode_gamepad) {
                if (zox_has(e2, Gamepad)) {
                    if (gamepad_is_any_input(world, e2)) {
                        using_current_inputs = 1;
                        break;
                    }
                }
            } else if (mode->value == zox_device_mode_touchscreen) {
                if (zox_has(e2, Touchscreen)) {
                    if (touchscreen_is_any_input(world, e2)) {
                        using_current_inputs = 1;
                        // zox_log(" > using current touchscreen\n")
                        break;
                    }
                }
            }
        }
        if (using_current_inputs) {
            if (dbg_log >= 2) {
                zox_log("Using Current Device [%i]", mode->value);
            }
            continue;
        }
        if (dbg_log >= 2) {
            zox_log("Devices %i; Mode [%i]", length, mode->value);
        }
        byte new_mode = 0;
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (dbg_log >= 2) {
                zox_log("   Device %i: %s", j, zox_get_name(e2));
            }
            if (mode->value != zox_device_mode_keyboardmouse) {
                if (zox_has(e2, Keyboard)) {
                    zox_geter(e2, Keyboard, keyboard)
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
                if (zox_has(e2, Gamepad)) {
                    if (gamepad_is_any_input(world, e2)) {
                        new_mode = zox_device_mode_gamepad;
                    }
                }
            }
            if (mode->value != zox_device_mode_touchscreen) {
                if (zox_has(e2, Touchscreen)) {
                    if (touchscreen_is_any_input(world, e2)) {
                        new_mode = zox_device_mode_touchscreen;
                    }
                }
            }
            if (new_mode) {
                break;
            }
        }
        if (!new_mode) {
            continue;
        }
        // set player links here if dirty
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (new_mode == zox_device_mode_keyboardmouse) {
                byte enabled = zox_has(e2, Keyboard) || zox_has(e2, Mouse);
                zox_set(e2, DeviceDisabled, { !enabled });
            } else if (new_mode == zox_device_mode_gamepad) {
                byte enabled = zox_has(e2, Gamepad);
                zox_set(e2, DeviceDisabled, { !enabled });
            } else if (new_mode == zox_device_mode_touchscreen) {
                byte enabled = zox_has(e2, Touchscreen);
                zox_set(e2, DeviceDisabled ,{ !enabled });
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
