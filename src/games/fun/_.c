#include "game_events.c"

static inline entity get_player_game(ecs* world, const entity player) {
    return zox_gett_value(player, GameLink);
}

void dispose_games(ecs* world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_events_games();
}