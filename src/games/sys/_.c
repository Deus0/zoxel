#include "state.c"

void zox_systems_games(ecs* world) {
    // NOTE: MainThread because triggers event
    zox_system_1(
        game_state_system,
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
