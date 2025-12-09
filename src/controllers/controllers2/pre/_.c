#include "character2_player.c"
entity prefab_player_character2;

void spawn_prefabs_players2D(ecs *world) {
    prefab_player_character2 = spawn_prefab_character2_player(world, prefab_character2D);
    prefab_game2_player = prefab_player_character2;
}