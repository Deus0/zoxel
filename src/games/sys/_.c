#include "state.c"

void zox_systems_games(ecs* world) {
    // main thread: a change adds the core Dirty, observers react (gam/*/eve, huds, terrain, weathers, players)
    zox_system_1(
        game_state_system,
        zoxp_spawn,
        [out] games.GameState,
        [out] games.GameStateTarget,
        [out] games.LastGameState,
        [out] games.GameStateWait,
        [out] games.GameStateTime,
        [none] games.Game
    );
    zox_on_add(game_state_realm_clock, [in] games.GameState, [none] games.Game, [none] core.Dirty);
}
