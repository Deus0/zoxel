void on_element_clicked(ecs *world, entity player, entity e) {
    if (!zox_valid(e) || !zox_has(e, Clickable)) {
        return;
    }
    if (zox_has(e, ClickDisabled) && zox_getv(e, ClickDisabled)) {
        return;
    }
    zox_set(e, ClickState, { zox_click_state_trigger_clicked });
    zox_set(e, Clicker, { player });
}

void on_element_released(ecs *world, entity player, entity e) {
    if (!zox_valid(e) || !zox_has(e, Clickable)) {
        return;
    }
    zox_set(e, ClickState, { zox_click_state_trigger_released });
    zox_set(e, Clicker, { player });
}

void set_raycast_target_children(ecs *world, entity e, entity target) {
    if (!zox_valid(e)) {
        return;
    }
    if (zox_has(e, RaycasterTarget)) {
        entity last_target = zox_get_value(e, RaycasterTarget)
        if (zox_valid(last_target)) {
            zox_set(last_target, SelectState, { zox_state_deselect_trigger });
            // zox_set(last_target, SelectStateDirty, { zox_dirty_trigger });
        }
        zox_set(e, RaycasterTarget, { target });
        if (zox_valid(target)) {
            zox_set(target, SelectState, { zox_state_select_trigger });
            // zox_set(target, SelectStateDirty, { zox_dirty_trigger });
        }
    }
    entity children[layouts2_children_capacity];
    uint count = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint i = 0; i < count; i++) {
        entity child = children[i];
        if (!zox_valid(child)) {
            continue;
        }
        set_raycast_target_children(world, child, target);
    }
    entity devices[zox_children_capacity];
    uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
    for (uint j = 0; j < length; j++) {
        entity e2 = devices[j];
        if (!zox_valid(e2)) {
            continue;
        }
        set_raycast_target_children(world, e2, target);
    }
}

void raycaster_select_element(ecs *world, entity raycaster, entity element) {
    set_raycast_target_children(world, raycaster, element);
}


void set_element_dragged(ecs *world, entity ndragger, entity e, byte drag_mode) {
    DraggableState* state = zox_get_mut(e, DraggableState);
    if (!state->value) {
        zox_muter(e, DraggerLink, dragger);
        dragger->value = ndragger;
        state->value = drag_mode;
        zox_modified(e, DraggableState);
        if (is_log_dragging) {
            zox_log("Dragging Started [%f]", (float) zox_current_time);
        }
    }
}
