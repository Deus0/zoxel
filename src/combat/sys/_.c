#include "death_cleanup.c"
#include "combat_states.c"

void define_systems_combat(ecs* world) {
    zox_system(
        CombatStateSystem,
        zoxp_update,
        [in] combat.LastCombatTime,
        [out] combat.CombatState
    );
    zox_system(
        DeathCleanSystem,
        zoxp_destroy,
        [in] core.DiedTime,
        [none] core.Dead,
    );
}
