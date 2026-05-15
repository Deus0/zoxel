// todo: SelectedEvent, Deselected Event, or, SelectState = SelectedThisFrame, Selected, Deselected, None
// todo: Give item a texture, spawn as icon!
zox_sys2(UserIconTooltipSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    zox_sys_in(DataLink);
    zox_sys_in(TooltipEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, selectState);
        zox_sys_i(DataLink, userDataLink);
        zox_sys_i(TooltipEvent, tooltipEvent);
        if (!(selectState->value == zox_select_state_selected_this_frame || selectState->value == zox_select_state_deselected_this_frame)) {
            continue;
        }
        if (!zox_valid(userDataLink->value) || !tooltipEvent->value) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
        if (!tooltip) {
            zox_loge("Tooltip not found in canvas");
            continue;
        }
        // deselect event here is fine
        if (selectState->value == zox_select_state_deselected_this_frame) {
            set_entity_text(world, tooltip, "");
            continue;
        }
        TooltipEventData data = {
            .event = selectState->value,
            .tooltip = tooltip,
            .data = userDataLink->value
        };
        (*tooltipEvent->value)(world, &data);
    }
} zox_sys_end(UserIconTooltipSystem);
