// NOTE: Link UI to its canvas tooltip when initialized.
zox_sys2(TooltipeeInitializeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Initialize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Initialize, initialize);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            return;
        }
        entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
        if (!zox_valid(tooltip)) {
            zox_loge("Tooltip not found in Canvas [%s]", zox_getn(canvas));
            return;
        }
        zox_link(world, e, TooltipLink, tooltip);
        if (dbg_log) {
            zox_log("Tooltipee Initialized [%s] => [%s]", zox_getn(e), zox_getn(tooltip));
        }
    }
} zox_sys_end(TooltipeeInitializeSystem);