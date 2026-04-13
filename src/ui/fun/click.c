void on_element_clicked(ecs *world, entity player, entity e) {
    if (!zox_valid(e) || !zox_has(e, Clickable)) {
        return;
    }

    if (zox_has(e, ClickDisabled) && zox_gett_value(e, ClickDisabled)) {
        return;
    }

    zox_set(e, ClickState, { zox_click_state_trigger_clicked });
    zox_set(e, Clicker, { player });
}

void on_element_released(ecs *world, entity player, entity e) {
    if (zox_valid(e) && zox_has(e, Clickable)) {
        zox_set(e, ClickState, { zox_click_state_trigger_released });
        zox_set(e, Clicker, { player });
    }
}

void set_raycast_target_children(ecs *world, entity e, entity target) {
    if (!zox_valid(e)) {
        return;
    }
    if (zox_has(e, RaycasterTarget)) {
        entity last_target = zox_get_value(e, RaycasterTarget)
        if (zox_valid(last_target)) {
            zox_set(last_target, SelectState, { zox_select_state_trigger_deselect });
        }
        zox_set(e, RaycasterTarget, { target });
        if (zox_valid(target)) {
            zox_set(target, SelectState, { zox_select_state_trigger_selected });
        }
    }
    if (zox_has(e, Children)) {
        zox_geter(e, Children, children);
        for (int i = 0; i < children->length; i++) {
            entity child = children->value[i];

            if (!zox_valid(child)) {
                continue;
            }
            set_raycast_target_children(world, child, target);
        }
    }
    if (zox_has(e, DeviceLinks)) {
        zox_geter(e, DeviceLinks, children)
        for (int i = 0; i < children->length; i++) {
            entity child = children->value[i];

            if (!zox_valid(child)) {
                continue;
            }
            set_raycast_target_children(world, child, target);
        }
    }
}

void raycaster_select_window_children(ecs *world, entity e, entity window) {
    if (zox_has(e, WindowRaycasted)) {
        zox_set(e, WindowRaycasted, { window })
    }
    if (zox_has(e, Children)) {
        zox_geter(e, Children, children)
        for (int i = 0; i < children->length; i++) {
            entity child = children->value[i];

            if (!child) continue;

            raycaster_select_window_children(world, child, window);
        }
    }
    if (zox_has(e, DeviceLinks)) {
        zox_geter(e, DeviceLinks, children)
        for (int i = 0; i < children->length; i++) {
            entity child = children->value[i];

            if (!child) continue;

            raycaster_select_window_children(world, child, window);
        }
    }
}

// here we pass down selected components to children

void raycaster_select_element(ecs *world, entity raycaster, entity element) {
    set_raycast_target_children(world, raycaster, element);
}

void raycaster_select_window(ecs *world, entity e, entity window) {
    raycaster_select_window_children(world, e, window);
}
