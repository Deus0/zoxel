#include "game_events.c"

static inline entity get_player_game(ecs* world, const entity player) {
    return zox_get_link(world, player, GameLink);
}

/*void dispose_games(ecs* world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_events_games();
}*/

// the game is ending (the end, its clean up, or set to), what streams or plays in it stops: what spawns now is
//  orphaned when the realm goes the same frame
static inline byte game_ending(ecs* world, entity game) {
    return !zox_valid(game) || zox_getv(game, GameState) >= zox_game_state_the_end ||
        zox_getv(game, GameStateTarget) >= zox_game_state_the_end;
}
