// todo: reuse parts of ZeviceClickSystem in this
// this is now from zevice
zox_sys2(DeviceClickSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(EntityTarget);
    zox_sys_out(ClickingEntity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(EntityTarget, target);
        zox_sys_o(ClickingEntity, clicking);
        if (!zox_valid(target->value) ||
            !zox_has(target->value, RenderDisabled) ||
            zox_getv(target->value, RenderDisabled)
        ) {
            continue;
        }
        entity player = zox_get_parent(world, e);
        if (!zox_valid(player)) {
            continue;
        }
        byte input_type = 0;
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(world, e, children, children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            if (!zox_has(e2, ZeviceButton) || !zox_has(e2, DeviceButtonType)) {
                continue;
            }
            if (zox_has(e2, Disabled)) {
                continue;
            }
            byte button_type = zox_getv(e2, DeviceButtonType);
            if (button_type == zox_btn_a) {
                byte clicked = zox_getv(e2, ZeviceButton);
                if (devices_get_pressed_this_frame(clicked)) {
                    input_type = 1;
                } else if (devices_get_released_this_frame(clicked)) {
                    input_type = 2;
                }
            }
        }
        if (!input_type) {
            continue;
        }
        // used for virtual joysticks to see if a t arget was raycasted, todo: move to raycast system
        // raycasterResult->value = target->value || windowRaycasted->value;
        // released
        // First Clicked
        if (input_type == 1) {
            clicking->value = target->value;
            on_element_clicked(world, player, clicking->value);
            if (dbg_log) {
                zox_log("Began Click [%s]", zox_getn(target->value));
            }
        } else if (input_type == 2) {
            // released
            if (target->value == clicking->value) {
                on_element_released(world, player, target->value);
            }
            clicking->value = 0;
            if (dbg_log) {
                zox_log("Released Click [%s]", zox_getn(target->value));
            }
        }
        if (input_type == 1) { // clicked
            if (zox_valid(target->value) && zox_has(target->value, Dragable)) {
                byte dmode = zox_getv(player, DeviceMode);
                byte drag_mode = zox_drag_mode_none;
                if (dmode == zox_device_mode_keyboardmouse) {
                    drag_mode = zox_drag_mode_mouse;
                } else if (dmode == zox_device_mode_touchscreen) {
                    drag_mode = zox_drag_mode_finger;
                }
                set_element_dragged(world, player, target->value, drag_mode);
                if (dbg_log) {
                    zox_log("Dragging UI [%s] Mode [%i]", zox_getn(target->value), drag_mode);
                }
            }
        }
    }
} zox_sys_end(DeviceClickSystem);
