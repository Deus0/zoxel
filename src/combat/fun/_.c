// zox_current_time
void combat_on_hit(
    ecs* world,
    entity e,
    entity e2
) {
    if (!zox_valid(e)) {
        return;
    }
    zox_set(e, LastDamager, { e2 });
    zox_geter_value(e, CombatState, byte, combat_state);

    if (!(combat_state == zox_combat_battle || combat_state == zox_combat_enter_battle)) {
        zox_set(e, CombatState, { zox_combat_trigger_battle })
    }

    // any exchanges we update their timer
    zox_set(e, LastCombatTime, { zox_current_time });
    zox_set(e2, LastCombatTime, { zox_current_time });
}