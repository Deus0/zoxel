// zox_current_time
void combat_on_hit(
    ecs* world,
    entity e,
    entity attacker)
{
    if (!zox_valid(e)) {
        return;
    }
    entity old_attacker = zox_get_link(world, e, LastDamager);
    if (old_attacker) {
        zox_unlink(world, e, LastDamager, old_attacker);
    }
    zox_link(world, e, LastDamager, attacker);
    byte combat_state = zox_getv(e, CombatState);
    if (!(combat_state == zox_combat_battle ||
        combat_state == zox_combat_enter_battle))
    {
        zox_setv(e, CombatState, zox_combat_trigger_battle);
    }
    // any exchanges we update their timer
    zox_setv(e, LastCombatTime, zox_current_time);
    zox_setv(attacker, LastCombatTime, zox_current_time);
}