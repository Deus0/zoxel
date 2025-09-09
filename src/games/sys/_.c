#include "state.c"

void define_systems_games(ecs* world) {
    // NOTE: MainThread because triggers event
    zox_system_1(
        GameStateSystem,
        EcsOnUpdate,
        [in] realms.RealmLink,
        [out] GameState,
        [out] GameStateTarget,
        [out] GameStateDirty,
        [out] GameStateTime,
        [none] Game
    );
}