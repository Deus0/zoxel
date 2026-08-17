entity spawn_net_room(
    ecs *world,
    int port)
{
    zox_instance(prefab_net_room);
    zox_name("net_room");
    zox_setv(e, NetPort, port);
    return e;
}