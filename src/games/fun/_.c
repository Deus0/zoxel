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

// paused, the realm's clock stops (its ClockScale 0), all under it with it (timing/sys/clock.c); playing it runs again
void game_state_realm_clock(ecs* world, entity game, byte state) {
    const entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm) || !zox_has(realm, ClockScale)) {
        return;
    }
    if (state == zox_game_state_paused) {
        zox_setv(realm, ClockScale, 0);
    } else if (state == zox_game_state_playing) {
        zox_setv(realm, ClockScale, 1);
    }
}
