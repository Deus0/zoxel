#include "state.c"

void zox_systems_games(ecs* world) {
    // NOTE: MainThread because triggers event
    zox_system_1(
        GameStateSystem,
        zoxp_spawn,
        [in] games.GameStateEvent,
        [out] games.GameState,
        [out] games.GameStateTarget,
        [out] games.LastGameState,
        [out] games.GameStateDirty,
        [out] games.GameStateTime,
        [none] games.Game
    );
}
