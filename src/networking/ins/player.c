entity spawn_net_player(
    ecs *world,
    entity prefab,
    int port,
    byte4 target_ip,
    int target_port
) {
    zox_instance(prefab);
    zox_name("net_player");
    zox_set(e, NetPort, { port });
    zox_set(e, TargetNetAddress, { target_ip });
    zox_set(e, TargetNetPort, { target_port });
    // set_new_socket(world, e, port);
    return e;
}