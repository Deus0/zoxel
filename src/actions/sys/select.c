// NOTE: Uses slot index to equip a new action
zox_sys2(ActiveActionSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ActionIndex);
    zox_sys_out(ActiveAction);
    zox_sys_out(ActiveActionDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ActionIndex, index);
        zox_sys_o(ActiveAction, aaction);
        zox_sys_o(ActiveActionDirty, dirty);
        if (dirty->value) {
            continue;
        }
        entity actionbar = zox_get_child_by_id(world, e, zox_id(Actionbar));
        if (!zox_valid(actionbar)) {
            if (dbg_log) {
                zox_log("Actionbar not found on character [%s]", zox_get_name(e));
            }
            continue;
        }
        entity action_slots[layouts2_children_capacity];
        uint action_slots_length = zox_get_children_by_id(world, actionbar, action_slots, layouts2_children_capacity, zox_id(Slot));
        if (!action_slots_length) {
            zox_loge("[%s] Has no Action Slots", zox_get_name(e));
            continue;
        }
        if (index->value >= action_slots_length) {
            zox_loge("[%s] Action selected is out of bounds [%i of %i]", zox_get_name(e), index->value, action_slots_length);
            continue;
        }
        entity action_slot = action_slots[index->value];
        if (!zox_valid(action_slot)) {
            zox_loge("Actionslot invalid");
            continue;
        }
        entity action = zox_gett_value(action_slot, DataLink);
        if (action != aaction->value) {
            aaction->value = action;
            dirty->value = zox_dirty_trigger;
            if (dbg_log) {
                zox_log("Character Action Set [%s]:[%s]", zox_get_name(e), zox_get_name(action));
            }
        }

    }
} zox_sys_end(ActiveActionSystem);
