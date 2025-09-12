entity spawn_prefab_net_player(ecs *world) {
    zox_prefab();
    zox_prefab_name("net_player");
    zox_add_tag(e, NetPlayer);
    zox_add_tag(e, PacketReciever);
    zox_add_tag(e, PacketSender);
    zox_prefab_add(e, NetAddress);
    zox_prefab_add(e, NetPort);
    zox_prefab_add(e, TargetNetAddress);
    zox_prefab_add(e, TargetNetPort);
    zox_prefab_set(e, SocketLink, { -1 });
    return e;
}