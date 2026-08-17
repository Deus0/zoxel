entity spawn_prefab_net_room(ecs *world) {
    zox_prefab();
    zox_prefab_name("net_room");
    zox_add(e, NetRoom);
    zox_add(e, NetAddress);
    zox_add(e, NetPort);
    zox_setv(e, SocketLink, -1);
    zox_add(e, PacketReciever);
    zox_add(e, PacketSender);
    zox_add(e, PacketQueue);
    return e;
}
