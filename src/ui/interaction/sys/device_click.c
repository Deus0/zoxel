// todo: reuse parts of ZeviceClickSystem in this
// this is now from zevice
zox_sys2(DeviceClickSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceDisabled);
    zox_sys_in(PlayerLink);
    zox_sys_in(RaycasterTarget);
    zox_sys_out(ClickingEntity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceDisabled, disabled);
        zox_sys_i(PlayerLink, playerLink);
        zox_sys_i(RaycasterTarget, raycasterTarget);
        zox_sys_o(ClickingEntity, clickingEntity);
        if (disabled->value) {
            continue;
        }
        entity player = playerLink->value;
        if (!zox_valid(player) || !zox_has(player, CanvasLink)) {
            continue;
        }
        entity canvas = zox_get_value(player, CanvasLink);
        if (!zox_valid(canvas)) {
            continue;
        }
        byte input_type = 0;
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(world, e, children, children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            if (!zox_valid(e2)) {
                continue;
            }
            if (!zox_has(e2, ZeviceButton)) continue;
            if (!zox_has(e2, DeviceButtonType)) continue;
            byte button_type = zox_get_value(e2, DeviceButtonType);
            if (button_type == zox_device_button_a) {
                byte disabled = zox_get_value(e2, ZeviceDisabled);
                if (!disabled) {
                    zox_geter_value(e2, ZeviceButton, byte, value);
                    if (devices_get_pressed_this_frame(value)) {
                        input_type = 1;
                    } else if (devices_get_released_this_frame(value)) {
                        input_type = 2;
                    }
                }
            }
        }
        if (!input_type) {
            continue;
        }
        zox_geter_value(player, DeviceMode, byte, dmode);
        // used for virtual joysticks to see if a t arget was raycasted, todo: move to raycast system
        // raycasterResult->value = raycasterTarget->value || windowRaycasted->value;
        // released
        if (input_type == 1) {
            clickingEntity->value = raycasterTarget->value; // clicked
            on_element_clicked(world, player, clickingEntity->value);
        }
        if (input_type == 1) { // clicked
            /*if (windowRaycasted->value != windowTarget->value) {
                windowTarget->value = windowRaycasted->value;
                // zox_log(" > player window target [%lu]\n", windowTarget->value)
                // todo: set it's window to top
                // now set WindowToTop entity to windowTarget->value
                // then next in canvas system, reset windows layers to top of window stack
                zox_set(canvas, WindowToTop, { windowTarget->value })
            }*/
            if (zox_valid(raycasterTarget->value) && zox_has(raycasterTarget->value, Dragable)) {
                byte drag_mode = zox_drag_mode_none;
                if (dmode == zox_device_mode_keyboardmouse) {
                    drag_mode = zox_drag_mode_mouse;
                } else if (dmode == zox_device_mode_touchscreen) {
                    drag_mode = zox_drag_mode_finger;
                }
                set_element_dragged(world, player, raycasterTarget->value, drag_mode);
            }
        } else if (input_type == 2) { // released
            if (raycasterTarget->value == clickingEntity->value) {
                on_element_released(world, player, raycasterTarget->value);
            }
            clickingEntity->value = 0;
        }
    }
} zox_sys_end(DeviceClickSystem);
