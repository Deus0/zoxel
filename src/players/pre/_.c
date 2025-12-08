#include "player.c"
entity prefab_player;

void spawn_prefabs_players(ecs *world) {
    prefab_player = spawn_prefab_player(world);
    zox_prefab_set_device(PlayerLink, { 0 });
    if (prefab_game) {
        zox_prefab_add(prefab_game, PlayerLinks);
    }
}
