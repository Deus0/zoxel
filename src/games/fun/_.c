#include "game_events.c"

static inline entity get_player_game(ecs* world, const entity player) {
    return zox_get_link(world, player, GameLink);
}

/*void dispose_games(ecs* world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_events_games();
}*/
