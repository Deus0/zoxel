entity spawn_net_player(
    ecs *world,
    int port,
    byte4 target_ip,
    int target_port
) {
    zox_instance(prefab_net_player);
    zox_name("net_player");
    zox_setv(e, NetPort, port);
    entity host_connection = spawn_net_connection(
        world,
        target_ip,
        target_port);
    zox_set_parent(world, host_connection, e);
    return e;
}