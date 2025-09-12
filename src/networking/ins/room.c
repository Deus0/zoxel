entity spawn_net_room(ecs *world, entity prefab, const int port) {
    zox_instance(prefab);
    zox_name("net_room");
    zox_set(e, NetPort, { port });
    // set_new_socket(world, e, port);
    return e;
}