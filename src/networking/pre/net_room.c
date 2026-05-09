entity spawn_prefab_net_room(ecs *world) {
    zox_prefab();
    zox_prefab_name("net_room");
    zox_add_tag(e, NetRoom);
    zox_add_tag(e, PacketReciever);
    zox_add_tag(e, PacketSender);
    zox_prefab_add(e, NetAddress);
    zox_prefab_add(e, NetPort);
    zox_prefab_set(e, SocketLink, { -1 });
    zox_prefab_set(e, Children, { 0, NULL });
    // zox_prefab_add(e, Children);
    return e;
}
