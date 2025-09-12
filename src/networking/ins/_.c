#include "player.c"
#include "room.c"

void initialize_networking(ecs* world) {
#ifdef zox_windows
    initialize_windows_sockets();
#else
    sockets_enabled = 1;
#endif
    if (server_mode) {
        spawn_net_room(world, prefab_net_room, SERVER_PORT);
    } else {
        spawn_net_player(world, prefab_net_player, PORT, server_ip, SERVER_PORT);
    }
    if (server_mode) {
        zox_log(" > network server mode activated")
    } else {
        zox_log(" > network client mode activated")
    }
}

void on_boot_networking(ecs_world_t* world, ecs_entity_t app) {
    (void) app;
    if (zox_is_networking) {
        initialize_networking(world);
    }
}