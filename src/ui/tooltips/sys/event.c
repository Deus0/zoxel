// a ui selected fills its tooltip by its TooltipEvent
void tooltip_select_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const TooltipEvent* tooltip_event = zox_get(e, TooltipEvent);
        if (!tooltip_event->value) {
            continue;
        }
        entity tooltip = zox_get_link(world, e, TooltipLink);
        if (!zox_valid(tooltip)) {
            continue;
        }
        TooltipEventData data = {
            .ui = e,
            .tooltip = tooltip
        };
        (*tooltip_event->value)(world, &data);
        if (dbg_log) {
            zox_log("Tooltipee Activated [%s] => [%s]",
                zox_getn(e),
                zox_getn(tooltip));
        }
    }
    zox_sys_on_end();
} zoxd_system(tooltip_select_system);
