// NOTE: Starts Warmups when action equiped
// TODO: Implement a CanActivate flag
zox_sys2(ActionActivateSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TriggerActionA);
    zox_sys_in(ActiveAction);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TriggerActionA, trigger);
        zox_sys_i(ActiveAction, action);
        if (trigger->value != zox_dirty_active) {
            continue;
        }
        entity e2 = action->value;
        if (!zox_valid(e2)) {
            if (dbg_log) {
                zox_log("[%s]'s Active Action is empty", zox_getn(e));
            }
            continue;
        }
        byte is_activate = zox_has(e2, Activate) ? zox_getv(e2, Activate) : 0;
        byte is_activate_begin = zox_has(e2, ActivateBegin) ? zox_getv(e2, ActivateBegin) : 0;
        byte warmup_state = zox_has(e2, WarmupState) ? zox_getv(e2, WarmupState) : 0;
        double warmup_at = zox_has(e2, WarmupAt) ? zox_getv(e2, WarmupAt) : 0;
        byte is_warmup = warmup_state || warmup_at;
        byte cooldown_state = zox_has(e2, CooldownState) ? zox_getv(e2, CooldownState) : 0;
        double cooldown_at = zox_has(e2, CooldownAt) ? zox_getv(e2, CooldownAt) : 0;
        byte is_cooldown = cooldown_state || cooldown_at;
        if (is_activate || is_activate_begin || is_warmup || is_cooldown) {
            continue;
        }
        zox_set(e2, ActivateBegin, { zox_dirty_trigger });
        if (dbg_log) {
            zox_log(" - [%s] Action [%s] Begins", zox_get_name(e), zox_get_name(e2));
        }
    }
} zox_sys_end(ActionActivateSystem);

/*entity actionbar = zox_get_child_by_id(world, e, zox_id(Actionbar));
if (!zox_valid(actionbar)) {
    continue;
}
entity actions[layouts2_children_capacity];
uint actions_length = zox_get_children_by_id(world, actionbar, actions, layouts2_children_capacity, zox_id(Slot));
if (index->value >= actions_length) {
    zox_loge("Action selected is out of bounds [%i of %i]", index->value, actions_length);
    continue;
}
entity action = actions[index->value];
// no action assigned
if (!zox_valid(action->value)) {
    if (dbg_log) {
        zox_log("Action Empty at [%i]", index->value);
    }
    continue;
}*/
