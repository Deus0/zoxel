#include "game_states.c"
#include "player_states.c"

void zox_tsts_players(ecs* world) {
    zox_tsts_game_states(world);
    zox_tsts_player_states(world);
}
