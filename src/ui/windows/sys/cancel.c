// NOTE: Player cancel the menu!
zox_sys2(CancelMenuSystem) {
    byte dbg_log = 0;
    byte cancel_type = zox_btn_b;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZeviceDisabled);
    zox_sys_in(DeviceButtonType);
    zox_sys_in(ZeviceButton);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZeviceDisabled, disabled);
        zox_sys_i(DeviceButtonType, type);
        zox_sys_i(ZeviceButton, clicked);
        if (disabled->value) {
            continue;
        }
        entity device = zox_get_parent(world, e);
        if (!zox_valid(device)) {
            continue;
        }
        entity target = zox_has(device, RaycasterTarget) ? zox_getv(device, RaycasterTarget) : 0;
        if (!target) {
            target = zox_has(e, RaycasterTarget) ? zox_getv(e, RaycasterTarget) : 0;
        }
        // check all zevices
        /*iter it2 = zox_children(world, device);
        while (zox_children_next(it2)) {
            for (int i = 0; i < it2.count; i++) {
                entity e2 = it2.entities[i];
                if (zox_has(e2, RaycasterTarget)) {
                    entity new_target = zox_getv(e2, RaycasterTarget);
                    if (zox_valid(new_target)) {
                        target = new_target;
                        break;
                    }
                }
            }
        }*/
        // zox_getv(device, RaycasterTarget);
        if (!zox_valid(target)) {
            // zox_log("No Raycast Target");
            continue;
        }
        if (dbg_log) {
            if (devices_get_released_this_frame(clicked->value)) {
                zox_log("Button Released with type [%i]", type->value);
            }
        }
        byte is_cancel = 0;
        if (type->value == cancel_type) {
            if (devices_get_released_this_frame(clicked->value)) {
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
                zox_log("Player [%s] Clicking [%s]", zox_getn(player), zox_getn(close_button), zox_getn(menu));
            }
            on_element_clicked(world, player, close_button);
            on_element_released(world, player, close_button);
        }
    }
} zox_sys_end(CancelMenuSystem);
