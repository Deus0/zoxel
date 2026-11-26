#include "game.c"
#include "state.c"

void zox_systems_players(ecs* world) {
    zox_on_add(
        player_game_state_event,
        [in] games.GameState,
        [in] games.LastGameState,
        [none] games.Game,
        [none] core.Dirty
    );
    zox_system(
        player_state_system,
        zoxp_update,
        [out] players.PlayerState,
        [out] players.PlayerStateDirty,
        [out] players.PlayerStateTimer,
        [in] ?players.PlayerStateTarget,
        [none] players.Player
    );
}
