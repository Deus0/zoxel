#include "packet.c"
#include "connection.c"
#include "net_player.c"
#include "net_room.c"
entity prefab_net_connection;
entity prefab_net_player;
entity prefab_net_room;

void spawn_prefabs_networking(ecs *world) {
    prefab_net_connection = spawn_prefab_net_connection(world);
    prefab_net_room = spawn_prefab_net_room(world);
    prefab_net_player = spawn_prefab_net_player(world);
}