// NOTE: When attacking, Select and Trigger the action
zox_sys2(AttackTriggerSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Behaviour);
    zox_sys_out(ActiveAction);
    zox_sys_out(ActiveActionDirty);
    zox_sys_out(TriggerActionA);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Behaviour, behaviour);
        zox_sys_o(ActiveAction, aaction);
        zox_sys_o(ActiveActionDirty, dirty);
        zox_sys_o(TriggerActionA, trigger);
        if (behaviour->value != zox_behaviour_attack || trigger->value) {
            continue;
        }
        if (dirty->value) {
            continue;
        }
        if (!zox_valid(aaction->value)) {
            aaction->value = zox_get_child_by_id(world, e, zox_id(Skill));
            dirty->value = 1;
            if (dbg_log) {
                zox_log("+ NPC [%s] has Equiped Skill [%s]", zox_get_name(e), zox_get_name(aaction->value));
            }
            continue;
        }
        trigger->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("+ NPC [%s] is Attacking! [%s]", zox_get_name(e), zox_get_name(aaction->value));
        }
    }
} zox_sys_end(AttackTriggerSystem);
