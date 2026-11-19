// zox_current_time
void combat_on_hit(
    ecs* world,
    entity e,
    entity attacker)
{
    if (!zox_valid(e)) {
        return;
    }
    // NOTE: AttackerLink is exclusive, linking replaces the old attacker
    if (zox_get_link(world, e, AttackerLink) != attacker) {
        zox_set_link(world, e, AttackerLink, attacker);
    }
    byte combat_state = zox_getv(e, CombatState);
    if (!(combat_state == zox_combat_battle ||
        combat_state == zox_combat_enter_battle))
    {
        zox_setv(e, CombatState, zox_combat_trigger_battle);
    }
    // any exchanges we update their timer
    // on their clocks (ClockTime: paused it doesn't run out)
    zox_setv(e, LastCombatTime, clock_time_of(world, e));
    zox_setv(attacker, LastCombatTime, clock_time_of(world, attacker));
}
