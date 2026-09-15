void spawn_prefabs_combat(ecs *world) {
    // stats / death
    if (zox_valid(prefab_character3)) {
        zox_prefab_character_set(LastCombatTime, { 0 });
        zox_prefab_character_set(CombatState, { zox_combat_peace });
    }
}
