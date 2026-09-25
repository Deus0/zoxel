

/*void close_ui_related_tooltip(ecs* world, entity e) {
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
}*/

// NOTE: Windows are passed in, we should check all uis in tree?
void close_ui_related_tooltip(ecs* world, entity e) {
    byte dbg_log = 0;
    entity tooltip = zox_get_link(world, e, TooltipLink);
    if (!zox_valid(tooltip)) {
        return;
    }
    // for all uis connected to tooltip
    uint node_index = 0;
    while (1) {
        entity old_ui = zox_get_link_index(
            world,
            tooltip,
            TooltipLink,
            node_index);
        if (!old_ui) {
            break;
        }
        node_index++;
        if (old_ui == e) {
            zox_unlink(world, tooltip, TooltipLink, old_ui);
            if (dbg_log) {
                zox_log("Tooltip unlinked [%s]",
                    zox_getn(old_ui));
            }
            break;
        }
    }
    // TODO: Check Children
    /*entity old_ui = zox_get_link(world, tooltip, TooltipLink);
    if (old_ui == e) {
        zox_unlink(world, tooltip, TooltipLink, old_ui);
    }*/
}

// NOTE: Called by the UI element with a tooltip event on it
zox_sys2(TooltipeeDeselectSystem) {
    zox_sys_world();
    // zox_sys_begin();
    // zox_sys_in(SelectState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        /*zox_sys_i(SelectState, state);
        if (state->value != zox_state_deselect_active) {
            continue;
        }*/
        close_ui_related_tooltip(world, e);
    }
} zox_sys_end(TooltipeeDeselectSystem);

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
