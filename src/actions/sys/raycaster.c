// NOTE: Uses slot index to equip a new action
zox_sys2(ActiveActionRangeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ActiveActionDirty);
    zox_sys_in(ActiveAction);
    zox_sys_out(RaycastRange);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ActiveActionDirty, dirty);
        zox_sys_i(ActiveAction, aaction);
        zox_sys_o(RaycastRange, range);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        float new_range;
        if (!zox_valid(aaction->value) || !zox_has(aaction->value, RaycastRange)) {
            new_range = 0;
        } else {
            new_range = zox_getv(aaction->value, RaycastRange);
        }
        if (range->value != new_range) {
            range->value = new_range;
            if (dbg_log) {
                zox_log("+ [%s] New Raycast Range set [%f]", zox_get_name(e), range->value);
            }
        }
    }
} zox_sys_end(ActiveActionRangeSystem);
