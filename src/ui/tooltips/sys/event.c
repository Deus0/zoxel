zox_sys2(TooltipEventSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    zox_sys_in(TooltipEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, selectState);
        zox_sys_i(TooltipEvent, tooltipEvent);
        if (!(selectState->value == zox_select_state_selected_this_frame || selectState->value == zox_select_state_deselected_this_frame)) {
            continue;
        }
        if (!tooltipEvent->value) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
        if (!tooltip) {
            zox_log("Tooltip not found in canvas");
            continue;
        }
        if (selectState->value == zox_select_state_deselected_this_frame) {
            set_entity_text(world, tooltip, "");
            continue;
        }
        TooltipEventData data = {
            .event = selectState->value,
            .tooltip = tooltip,
            .triggered = e
        };
        if ((*tooltipEvent->value)(world, &data)) {
            // hmm need to rethink this
        }
    }
} zox_sys_end(TooltipEventSystem);
