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
