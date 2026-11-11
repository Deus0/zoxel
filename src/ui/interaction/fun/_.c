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
    zox_add(e, TriggerClick);
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

// A pointer, key or button went down on an element
//  - elements click when released on (on_element_press_released), so a press can be cancelled
//      by moving off it, unless they're ClickOnPress (touch controls, item slots)
//  - dragging starts on press, in the click systems
void on_element_pressed(
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
    zox_add(e, TriggerPress);
    if (zox_has(e, ClickOnPress)) {
        on_element_clicked(world, player, e);
    }
}

// Released on the same element it was pressed on, this is the click
void on_element_press_released(
    ecs *world,
    entity player,
    entity e)
{
    if (zox_valid(e) && !zox_has(e, ClickOnPress)) {
        on_element_clicked(world, player, e);
    }
    on_element_released(world, player, e);
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
        if (zox_valid(last_target) &&
            zox_has(last_target, Selectable))
        {
            zox_setm(last_target, SelectState, zox_state_deselect_trigger);
            zox_add(last_target, TriggerDeselect);
        }
        zox_setm(e, EntityTarget, target);
        if (zox_valid(target) &&
            zox_has(target, Selectable)
        ) {
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
    set_raycast_target_children(
        world,
        raycaster,
        element);
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
        zox_link(world, e, DraggerLink, dragger);
        if (is_log_dragging) {
            zox_log("Dragging Started!");
        }
    }
}
