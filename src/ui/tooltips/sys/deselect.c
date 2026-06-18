void close_ui_related_tooltip(ecs* world, entity e) {
    entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
    if (!zox_valid(canvas)) {
        return;
    }
    entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
    if (!zox_valid(tooltip)) {
        zox_loge("Tooltip not found in Canvas [%s]", zox_get_name(canvas));
        return;
    }
    set_entity_text(world, tooltip, "");
}

// NOTE: Called by the UI element with a tooltip event on it
zox_sys2(TooltipDeselectSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, state);
        if (state->value != zox_state_deselect_active) {
            continue;
        }
        close_ui_related_tooltip(world, e);
        /*entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
        if (!zox_valid(tooltip)) {
            zox_loge("Tooltip not found in Canvas [%s]", zox_get_name(canvas));
            continue;
        }
        set_entity_text(world, tooltip, "");*/
    }
} zox_sys_end(TooltipDeselectSystem);
