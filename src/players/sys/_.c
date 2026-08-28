#include "state.c"
#include "game.c"

void zox_define_systems_players(ecs* world) {
    zox_system_1(
        PlayerStateEventSystem,
        zoxp_spawn,
        [in] players.PlayerState,
        [in] players.PlayerStateDirty,
        [in] players.PlayerStateEvent,
        [none] players.Player
    );
    zox_system(
        PlayerStateSystem,
        zoxp_update,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [out] players.PlayerStateTimer,
        [none] players.Player
    );
    zox_system(
        PlayerGameStateSystem,
        zoxp_update,
        [in] games.GameStateDirty,
        [in] games.GameState,
        [in] games.LastGameState,
        [none] games.Game
    );
}
