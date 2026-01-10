extern void canvas_select_first_button(ecs *world, const entity raycaster, const entity canvas);

// #define zox_debug_navigation

// todo: fix this, RaycasterTarget moved to zevices
zox_sys2(ElementNavigationSystem) {
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLinks);
    zox_sys_in(DeviceMode);
    zox_sys_out(NavigatorState);
    zox_sys_out(NavigatorTimer);
    zox_sys_out(RaycasterTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceLinks, devices);
        zox_sys_i(DeviceMode, deviceMode);
        zox_sys_o(RaycasterTarget, raycasterTarget);
        zox_sys_o(NavigatorState, navigatorState);
        zox_sys_o(NavigatorTimer, navigatorTimer);

        byte device_mode = deviceMode->value;
#ifdef zox_debug_navigation
        device_mode = zox_device_mode_gamepad;
#endif

        if (device_mode != zox_device_mode_gamepad) {
            if (!navigatorState->value) {
                navigatorState->value = 1;
                navigatorTimer->value = 0;
            }
            continue;
        } else if (device_mode == zox_device_mode_gamepad && !raycasterTarget->value) {
            zox_sys_e()
            const entity canvas = zox_get_value(e, CanvasLink)
            canvas_select_first_button(world, e, canvas);
        }

        if (!raycasterTarget->value || !zox_alive(raycasterTarget->value)) {
            continue;
        }

        float2 left_stick = float2_zero;
        for (int j = 0; j < devices->length; j++) {
            const entity device = devices->value[j];

            if (!zox_valid(device) || zox_gett_value(device, DeviceDisabled)) {
                continue;
            }

            if (zox_has(device, Keyboard)) {
                zox_geter(device, Keyboard, keyboard)
                if (keyboard->down.pressed_this_frame) {
                    left_stick.y -= 1;
                } else if (keyboard->up.pressed_this_frame) {
                    left_stick.y += 1;
                } else  if (keyboard->left.pressed_this_frame) {
                    left_stick.x -= 1;
                } else if (keyboard->right.pressed_this_frame) {
                    left_stick.x += 1;
                }
            } else if (zox_has(device, Gamepad)) {
                zox_geter(device, Children, zevices);

                for (int k = 0; k < zevices->length; k++) {
                    const entity zevice = zevices->value[k];

                    if (!zox_valid(zevice) || !zox_has(zevice, ZeviceStick)) {
                        continue;
                    }


                    zox_geter(zevice, ZeviceDisabled, disabled);
                    if (disabled->value) {
                        continue;
                    }
                    zox_geter(zevice, ZeviceStick, stick);
                    left_stick.x += stick->value.x;
                    left_stick.y += stick->value.y;
                }
            }
        }
        // here cut
        if (float_abs(left_stick.y) < restore_joystick_cutoff) {
            navigatorState->value = 0;
            continue;
        }
        // if stops input
        if (navigatorState->value == 1) {
            // here we are waiting for gamepad to stop moving, as didn't start within gamepad mode
            continue;
        } else if (float_abs(left_stick.y) < ui_navigation_joystick_cutoff) {
            navigatorTimer->value = 0;
            continue;
        } else if (navigatorTimer->value > 0) {
            navigatorTimer->value -= delta_time;
            if (navigatorTimer->value < 0) {
                navigatorTimer->value = 0;
            }
            continue;
        }

        // using selected window, we navigation elements of that... this could be done better
        zox_geter_value(raycasterTarget->value, ParentLink, entity, parent);
        zox_geter(parent, Children, children);

        unsigned did_find = 0;
        for (int k = 0; k < children->length; k++) {
            const entity child = children->value[k];

            if (!zox_valid(child) || child != raycasterTarget->value || !zox_has(child, Selectable)) {
                continue;
            }

            did_find = 1;
            if (left_stick.y >= ui_navigation_joystick_cutoff && k > 1) {
                raycaster_select_element(world, e, children->value[k - 1]);
            } else if (left_stick.y <= -ui_navigation_joystick_cutoff && k != children->length - 1) {
                raycaster_select_element(world, e, children->value[k + 1]);
            }
            break;
        }
        if (did_find) {
            if (navigatorTimer->value < -ui_navigation_timing / 2) {
                navigatorTimer->value = ui_navigation_timing;
            } else {
                navigatorTimer->value += ui_navigation_timing;
            }
        }
    }
} zox_sys_end(ElementNavigationSystem);