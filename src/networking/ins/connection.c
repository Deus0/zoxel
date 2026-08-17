entity spawn_net_connection(
    ecs *world,
    byte4 target_ip,
    int target_port
) {
    zox_instance(prefab_net_connection);
    zox_name("net_connection");
    zox_setv(e, TargetNetAddress, target_ip);
    zox_setv(e, TargetNetPort, target_port);
    return e;
}