// NOTE: When attacking, Select and Trigger the action
zox_sys2(AttackTriggerSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Behaviour);
    zox_sys_in(DisableMovement);
    zox_sys_out(ActiveAction);
    zox_sys_out(TriggerActionA);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Behaviour, behaviour);
        zox_sys_i(DisableMovement, disable);
        zox_sys_o(ActiveAction, aaction);
        zox_sys_o(TriggerActionA, trigger);
        if (disable->value || behaviour->value != zox_behaviour_attack || trigger->value) {
            continue;
        }
        if (!zox_valid(aaction->value)) {
            aaction->value = zox_get_child_by_id(world, e, zox_id(Skill));
        }
        trigger->value = zox_dirty_trigger;
        // zox_log("Triggering NPC Attack");
    }
} zox_sys_end(AttackTriggerSystem);
