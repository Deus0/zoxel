#include "death_cleanup.c"
#include "combat_states.c"

void zox_systems_combat(ecs* world) {
    zox_system(
        combat_state_system,
        zoxp_update,
        [in] combat.LastCombatTime,
        [out] combat.CombatState
    );
    zox_system(
        death_clean_system,
        zoxp_destroy,
        [in] core.DiedTime,
        [none] core.Dead,
    );
}
