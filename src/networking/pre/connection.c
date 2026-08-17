entity spawn_prefab_net_connection(ecs* world) {
    zox_prefab();
    zox_prefab_name("net_connection");
    zox_add(e, NetConnection);
    zox_add(e, TargetNetAddress);
    zox_add(e, TargetNetPort);
    zox_add(e, PacketReciever);
    zox_add(e, PacketSender);
    return e;
}
