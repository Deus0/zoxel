// right click = place
zox_sys2(ActionActivateSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TriggerActionA);
    zox_sys_in(ActionIndex);
    zox_sys_in(ActionLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TriggerActionA, trigger);
        zox_sys_i(ActionIndex, index);
        zox_sys_i(ActionLinks, actions);
        if (trigger->value != zox_dirty_active) {
            continue;
        }
        if (index->value >= actions->length) {
            zox_loge("Action selected is out of bounds [%i of %i]", index->value, actions->length);
            continue;
        }
        entity action = actions->value[index->value];
        // no action assigned
        if (!zox_valid(action)) {
            if (dbg_log) {
                zox_log("Action Empty at [%i]", index->value);
            }
            continue;
        }
        // TODO: Replace with CanActivate later
        byte is_activate = zox_has(action, Activate) ? zox_gett_value(action, Activate) : 0;
        byte is_activate_begin = zox_has(action, ActivateBegin) ? zox_gett_value(action, ActivateBegin) : 0;
        byte warmup_state = zox_has(action, WarmupState) ? zox_gett_value(action, WarmupState) : 0;
        double warmup_at = zox_has(action, WarmupAt) ? zox_gett_value(action, WarmupAt) : 0;
        byte is_warmup = warmup_state || warmup_at;
        byte cooldown_state = zox_has(action, CooldownState) ? zox_gett_value(action, CooldownState) : 0;
        double cooldown_at = zox_has(action, CooldownAt) ? zox_gett_value(action, CooldownAt) : 0;
        byte is_cooldown = cooldown_state || cooldown_at;
        if (is_activate || is_activate_begin || is_warmup || is_cooldown) {
            continue;
        }
        zox_set(action, ActivateBegin, { zox_dirty_trigger });
        if (dbg_log) {
            zox_sys_e();
            zox_log(" - [%s] Action [%s] Begins", zox_get_name(e), zox_get_name(action));
        }
    }
} zox_sys_end(ActionActivateSystem);
