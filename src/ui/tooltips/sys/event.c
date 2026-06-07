zox_sys2(TooltipEventSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    zox_sys_in(TooltipEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, state);
        zox_sys_i(TooltipEvent, tooltipEvent);
        if (!(state->value == zox_state_select_active || state->value == zox_state_deselect_active)) {
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
        if (state->value == zox_state_deselect_active) {
            set_entity_text(world, tooltip, "");
            continue;
        }
        TooltipEventData data = {
            .event = state->value,
            .tooltip = tooltip,
            .triggered = e
        };
        if ((*tooltipEvent->value)(world, &data)) {
            // hmm need to rethink this
        }
    }
} zox_sys_end(TooltipEventSystem);
