// NOTE: System responsible for device switching for a DeviceUser
// TODO: Grab not used devices from DeviceManager instead of local ones
zox_sys2(DeviceSwitchSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    if (!auto_switch_device) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(DeviceMode);
    zox_sys_out(DeviceModeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceMode, state);
        zox_sys_o(DeviceModeDirty, dirty);
        // first check if currently using selected inputs
        byte using_current_inputs = 0;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!e2) {
                continue;
            }
            if (state->value == zox_device_mode_keyboardmouse) {
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
            } else if (state->value == zox_device_mode_gamepad) {
                if (zox_has(e2, Gamepad)) {
                    if (gamepad_is_any_input(world, e2)) {
                        using_current_inputs = 1;
                        break;
                    }
                }
            } else if (state->value == zox_device_mode_touchscreen) {
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
            continue;
        }
        byte old_device_mode = state->value;
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!e2) {
                continue;
            }
            if (state->value != zox_device_mode_keyboardmouse) {
                if (zox_has(e2, Keyboard)) {
                    zox_geter(e2, Keyboard, keyboard)
                    if (keyboard_is_any_input(keyboard)) {
                        dirty->value = zox_device_mode_keyboardmouse;
                    } else {
                        continue;
                    }
                } else if (zox_has(e2, Mouse)) {
                    if (mouse_is_any_input(world, e2)) {
                        dirty->value = zox_device_mode_keyboardmouse;
                    } else {
                        continue;
                    }
                }
            }
            if (state->value != zox_device_mode_gamepad) {
                if (zox_has(e2, Gamepad)) {
                    if (gamepad_is_any_input(world, e2)) {
                        dirty->value = zox_device_mode_gamepad;
                    } else {
                        continue;
                    }
                }
            }
            if (state->value != zox_device_mode_touchscreen) {
                if (zox_has(e2, Touchscreen)) {
                    if (touchscreen_is_any_input(world, e2)) {
                        dirty->value = zox_device_mode_touchscreen;
                    } else {
                        continue;
                    }
                }
            }
            if (dirty->value) {
                if (dbg_log) {
                    zox_log("device mode changing: %i to %i", state->value, dirty->value);
                }
                break;
            }
        }
        if (!dirty->value) {
            continue;
        }
        // set player links here if dirty
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (!zox_valid(e2)) {
                continue;
            }
            if (dirty->value == zox_device_mode_keyboardmouse) {
                byte enabled = zox_has(e2, Keyboard) || zox_has(e2, Mouse);
                zox_set(e2, DeviceDisabled, { !enabled });
            } else if (dirty->value == zox_device_mode_gamepad) {
                byte enabled = zox_has(e2, Gamepad);
                zox_set(e2, DeviceDisabled, { !enabled });
            } else if (dirty->value == zox_device_mode_touchscreen) {
                byte enabled = zox_has(e2, Touchscreen);
                zox_set(e2, DeviceDisabled ,{ !enabled });
            }
        }
        if (dbg_log && dirty->value && old_device_mode != dirty->value) {
            // zox_log_input(" + device mode switched to [%i] from [%i]", dirty->value, deviceMode->value)
            if (dirty->value == zox_device_mode_keyboardmouse) {
                zox_log("> device mode changed to [keyboardmouse]")
            } else if (dirty->value == zox_device_mode_touchscreen) {
                zox_log("> device mode changed to [touchscreen]")
            } else if (dirty->value == zox_device_mode_gamepad) {
                zox_log("> device mode changed to [gamepad]")
            }
        }
    }
} zox_sys_end(DeviceSwitchSystem);
