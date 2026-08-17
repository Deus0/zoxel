entity spawn_prefab_net_player(ecs *world) {
    zox_prefab();
    zox_prefab_name("net_player");
    zox_add(e, NetPlayer);
    zox_add(e, PacketReciever);
    zox_add(e, PacketSender);
    zox_add(e, NetAddress);
    // these are linked
    zox_add(e, NetPort);
    zox_setv(e, SocketLink, -1);
    // zox_add(e, TargetNetAddress);
    // zox_add(e, TargetNetPort);
    zox_add(e, PacketQueue);
    return e;
}