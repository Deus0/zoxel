zox_sys2(TooltipEventSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    zox_sys_in(TooltipEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, state);
        zox_sys_i(TooltipEvent, tooltip_event);
        if (state->value != zox_state_select_active) {
            continue;
        }
        /*if (!(state->value == zox_state_select_active ||
            state->value == zox_state_deselect_active)) {
            continue;
        }*/
        if (!tooltip_event->value) {
            continue;
        }
        /*entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }*/
        /*entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
        if (!tooltip) {
            zox_log("Tooltip not found in canvas");
            continue;
        }*/
        /*if (state->value == zox_state_deselect_active) {
            // set_entity_text(world, tooltip, "");
            continue;
        }*/
        entity tooltip = zox_get_link(world, e, TooltipLink);
        if (!zox_valid(tooltip)) {
            return;
        }
        // TODO: Make another system that just uses TooltipText instead of event
        TooltipEventData data = {
            // .event = state->value,
            .ui = e,
            .tooltip = tooltip
        };
        if ((*tooltip_event->value)(world, &data)) {
            // hmm need to rethink this
        }
        if (dbg_log) {
            zox_log("Tooltipee Activated [%s] => [%s]", zox_getn(e), zox_getn(tooltip));
        }
    }
} zox_sys_end(TooltipEventSystem);
