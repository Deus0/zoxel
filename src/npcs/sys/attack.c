zox_sys2(AttackSystem) {
    zox_sys_begin();
    zox_sys_in(Behaviour);
    zox_sys_in(DisableMovement);
    zox_sys_in(ThreatPosition);
    zox_sys_out(TargetPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Behaviour, behaviour);
        zox_sys_i(DisableMovement, disable);
        zox_sys_i(ThreatPosition, threat);
        zox_sys_o(TargetPosition, target);
        if (disable->value || behaviour->value != zox_behaviour_attack) {
            continue;
        }
        target->value = threat->value;
    }
} zox_sys_end(AttackSystem);

// When attacking, trigger the action
zox_sys2(AttackTriggerSystem) {
    zox_sys_begin();
    zox_sys_in(Behaviour);
    zox_sys_in(DisableMovement);
    zox_sys_out(TriggerActionA);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Behaviour, behaviour);
        zox_sys_i(DisableMovement, disable);
        zox_sys_o(TriggerActionA, trigger);
        if (disable->value || behaviour->value != zox_behaviour_attack || trigger->value) {
            continue;
        }
        trigger->value = zox_dirty_trigger;
        // zox_log("Triggering NPC Attack");
    }
} zox_sys_end(AttackTriggerSystem);
