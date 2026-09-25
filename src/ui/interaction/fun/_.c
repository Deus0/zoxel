void on_element_clicked(
    ecs *world,
    entity player,
    entity e)
{
    if (!zox_valid(e) || !zox_has(e, Clickable)) {
        return;
    }
    if (zox_has(e, ClickDisabled) && zox_getv(e, ClickDisabled)) {
        return;
    }
    zox_setm(e, ClickState, zox_click_state_trigger_clicked);
    zox_setm(e, Clicker, player);
}

void on_element_released(
    ecs *world,
    entity player,
    entity e)
{
    if (!zox_valid(e) || !zox_has(e, Clickable)) {
        return;
    }
    zox_setm(e, ClickState, zox_click_state_trigger_released);
    zox_setm(e, Clicker, player);
}

void set_raycast_target_children(
    ecs *world,
    entity e,
    entity target)
{
    if (!zox_valid(e)) {
        return;
    }
    if (zox_has(e, EntityTarget)) {
        entity last_target = zox_getv(e, EntityTarget);
        if (zox_valid(last_target) && zox_has(last_target, SelectState)) {
            zox_setm(last_target, SelectState, zox_state_deselect_trigger);
            zox_add(last_target, TriggerDeselect);
        }
        zox_setm(e, EntityTarget, target);
        if (zox_valid(target)) {
            zox_setm(target, SelectState, zox_state_select_trigger);
            zox_add(target, TriggerSelect);
        }
    }
    entity children[layouts2_children_capacity];
    uint count = zox_get_children(
        world,
        e,
        children,
        layouts2_children_capacity);
    for (uint i = 0; i < count; i++) {
        entity child = children[i];
        set_raycast_target_children(
            world,
            child,
            target);
    }
}

void raycaster_select_element(
    ecs *world,
    entity raycaster,
    entity element)
{
    set_raycast_target_children(world, raycaster, element);
}


void set_element_dragged(
    ecs *world,
    entity dragger,
    entity e,
    byte drag_mode)
{
    DraggableState* state = zox_get_mut(e, DraggableState);
    if (!state->value) {
        state->value = drag_mode;
        zox_setm(e, DraggerLink, dragger);
        if (is_log_dragging) {
            zox_log("Dragging Started!");
        }
    }
}
