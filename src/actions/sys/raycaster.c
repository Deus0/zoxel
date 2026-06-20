// NOTE: Uses slot index to equip a new action
zox_sys2(ActiveActionRangeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ActiveActionDirty);
    zox_sys_in(ActiveAction);
    zox_sys_out(RaycastRange);
    zox_sys_out(RaycastType);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ActiveActionDirty, dirty);
        zox_sys_i(ActiveAction, aaction);
        zox_sys_o(RaycastRange, range);
        zox_sys_o(RaycastType, rtype);
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
        float new_type;
        if (!zox_valid(aaction->value) || !zox_has(aaction->value, RaycastType)) {
            new_type = 0;
        } else {
            new_type = zox_getv(aaction->value, RaycastType);
        }
        if (rtype->value != new_type) {
            rtype->value = new_type;
            if (dbg_log) {
                zox_log("+ [%s] New Raycast Type set [%f]", zox_get_name(e), rtype->value);
            }
        }
    }
} zox_sys_end(ActiveActionRangeSystem);
