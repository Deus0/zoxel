

// NOTE: Windows are passed in, we should check all uis in tree?
void close_ui_related_tooltip(ecs* world, entity e) {
    byte dbg_log = zox_log_tooltips;
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
}

// a ui deselected closes its tooltip (an observer on Deselect)
void tooltipee_deselect_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        close_ui_related_tooltip(world, e);
    }
}
