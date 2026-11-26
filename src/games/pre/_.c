entity prefab_game;
entity local_game;

#include "game.c"

void spawn_prefabs_games(ecs *world) {
    prefab_game = spawn_prefab_game(world);
}
