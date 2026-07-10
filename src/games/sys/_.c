#include "state.c"

void zox_define_systems_games(ecs* world) {
    // NOTE: MainThread because triggers event
    zox_system(
        GameStateSystem,
        zoxp_update, // EcsPostUpdate,
        [in] realms.RealmLink,
        [out] games.GameState,
        [out] games.GameStateTarget,
        [out] games.LastGameState,
        [out] games.GameStateDirty,
        [out] games.GameStateTime,
        [none] games.Game
    );
}
