entity spawn_prefab_net_room(ecs *world) {
    zox_prefab();
    zox_prefab_name("net_room");
    zox_add(e, NetRoom);
    zox_add(e, PacketReciever);
    zox_add(e, PacketSender);
    zox_prefab_add(e, NetAddress);
    zox_prefab_add(e, NetPort);
    zox_prefab_set(e, SocketLink, { -1 });
    return e;
}
