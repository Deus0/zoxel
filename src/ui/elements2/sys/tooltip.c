zox_sys2(TooltipSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    zox_sys_in(CanvasLink);
    zox_sys_in(TooltipEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, selectState);
        zox_sys_i(TooltipEvent, tooltipEvent);
        zox_sys_i(CanvasLink, canvas);
        if (!zox_valid(canvas->value)) {
            // sets in system now
            // zox_logw("Canvas Missing on [%s]", zox_get_name(e));
            continue;
        }
        if (!(selectState->value == zox_select_state_selected_this_frame || selectState->value == zox_select_state_deselected_this_frame)) {
            continue;
        }
        if (!tooltipEvent->value) {
            continue;
        }
        entity tooltip = find_child_with_tag_recursive(world, canvas->value, zox_id(Tooltip));
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
} zox_sys_end(TooltipSystem);
