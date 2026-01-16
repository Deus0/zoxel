// TODO: Move this to Triggers Module
// TODO: Make use hotkeys instead, link hotkeys to actions
// TODO: Refactor the DisableMovement checks for a and b

// NOTE: Player Input -> Character Triggers
zox_sys2(Player3DTriggerSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLinks);
    zox_sys_in(DeviceMode);
    zox_sys_in(CharacterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CharacterLink, character_link);
        zox_sys_i(DeviceMode, mode);
        zox_sys_i(DeviceLinks, devices);

        entity character = character_link->value;
        if (!zox_valid(character) || !zox_has(character, Character3)) {
            continue;
        }

        byte is_triggered_a = 0;
        byte is_triggered_b = 0;
        byte is_triggered_e = 0;
        for (int j = 0; j < devices->length; j++) {
            const entity device = devices->value[j];
            if (!zox_valid(device) || !zox_has(device, DeviceDisabled) || zox_gett_value(device, DeviceDisabled)) {
                continue;
            }
            if (mode->value == zox_device_mode_keyboardmouse && zox_has(device, Mouse)) {
                zox_geter(device, Children, zevices);
                for (int k = 0; k < zevices->length; k++) {
                    const entity zevice = zevices->value[k];
                    if (zox_has(zevice, ZevicePointer)) {
                        zox_geter_value(zevice, ZevicePointer, byte, click);
                        // if (devices_get_pressed_this_frame(click)) {
                        if (devices_get_pressed(click)) {
                            is_triggered_a = 1;
                        }
                    }
                    if (zox_has(zevice, ZevicePointerRight)) {
                        zox_geter_value(zevice, ZevicePointerRight, byte, click);
                        // if (devices_get_pressed_this_frame(click)) {
                        if (devices_get_pressed_this_frame(click)) {
                            is_triggered_b = 1;
                        }
                    }
                }
            } else if (mode->value == zox_device_mode_keyboardmouse && zox_has(device, Keyboard)) {
                zox_geter(device, Keyboard, keyboard);
                if (keyboard->escape.pressed_this_frame) {
                    is_triggered_e = 1;
                }
            } else if (mode->value == zox_device_mode_gamepad && zox_has(device, Gamepad)) {
                zox_geter(device, Children, children);
                for (int k = 0; k < children->length; k++) {
                    entity zevice = children->value[k];
                    if (zox_has(zevice, ZeviceButton)) {
                        zox_geter_value(zevice, DeviceButtonType, byte, type);
                        if (type == zox_device_button_x || type == zox_device_button_rt) {
                            zox_geter_value(zevice, ZeviceDisabled, byte, disabled);
                            if (disabled) {
                                continue;
                            }
                            zox_geter_value(zevice, ZeviceButton, byte, value);
                            if (devices_get_pressed_this_frame(value)) {
                                is_triggered_a = 1;
                            }
                        } else if (type == zox_device_button_y || type == zox_device_button_lt) {
                            zox_geter_value(zevice, ZeviceDisabled, byte, disabled);
                            if (disabled) {
                                continue;
                            }
                            zox_geter_value(zevice, ZeviceButton, byte, value);
                            if (devices_get_pressed_this_frame(value)) {
                                 is_triggered_b = 1;
                            }
                        }
                    }
                }
            }
        }

        if (is_triggered_e && !zox_gett_value(character, TriggerActionE)) {
            zox_set(character, TriggerActionE, { zox_dirty_trigger });
        }

        zox_geter_value(character, DisableMovement, byte, disabled);
        if (!disabled) {
            if (is_triggered_a && !zox_gett_value(character, TriggerActionA)) {
                zox_set(character, TriggerActionA, { zox_dirty_trigger });
            }
            if (is_triggered_b && !zox_gett_value(character, TriggerActionB)) {
                zox_set(character, TriggerActionB, { zox_dirty_trigger });
            }
        }
    }
} zox_sys_end(Player3DTriggerSystem);