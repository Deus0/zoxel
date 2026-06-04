// NOTE: Player cancel the menu!
zox_sys2(CancelMenuSystem) {
    byte dbg_log = 0;
    byte cancel_type = zox_device_button_b;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZeviceDisabled);
    zox_sys_in(DeviceButtonType);
    zox_sys_in(ZeviceButton);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZeviceDisabled, disabled);
        zox_sys_i(DeviceButtonType, type);
        zox_sys_i(ZeviceButton, button);
        if (disabled->value) {
            continue;
        }
        entity device = zox_get_parent(world, e);
        if (!zox_valid(device)) {
            continue;
        }
        entity target = zox_getv(device, RaycasterTarget);
        if (!zox_valid(target)) {
            // zox_log("No Raycast Target");
            continue;
        }
        if (dbg_log) {
            if (devices_get_released_this_frame(button->value)) {
                zox_log("Button Released with type [%i]", type->value);
            }
        }
        byte is_cancel = 0;
        if (type->value == cancel_type) {
            if (devices_get_released_this_frame(button->value)) {
                is_cancel = 1;
            }
        }
        // released
        if (is_cancel) {
            entity menu = zox_get_parent_by_id(world, target, zox_id(Window));
            if (!zox_valid(menu)) {
                continue;
            }
            // zox_log("Canceling Menu [%s]", zox_get_name(menu));
            entity close_button = zox_get_child_by_id_recursive(world, menu, zox_id(CloseButton));
            if (!zox_valid(close_button)) {
                // zox_log("No CloseButton found.");
                continue;
            }
            entity player = zox_get_parent(world, device);
            // clicked state?
            if (dbg_log) {
                zox_log("Player [%s] Clicking [%s]", zox_get_name(player), zox_get_name(close_button), zox_get_name(menu));
            }
            on_element_clicked(world, player, close_button);
            on_element_released(world, player, close_button);
        }
    }
} zox_sys_end(CancelMenuSystem);
