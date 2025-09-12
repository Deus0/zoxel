zox_tag(Packet);
zox_tag(NetRoom);
zox_tag(NetPlayer);
zox_tag(PacketReciever);
zox_tag(PacketSender);
zoxc(NetAddress, byte4);
zoxc(TargetNetAddress, byte4);
zoxc_int(NetPort);
zoxc_int(TargetNetPort);
zoxc_int(SocketLink);
zoxc_arrayd(PacketData, byte)
#include "socket_link.c"

void define_components_networking(ecs* world) {
    zoxd_tag(Packet);
    zoxd_tag(NetRoom);
    zoxd_tag(NetPlayer);
    zoxd_tag(PacketReciever);
    zoxd_tag(PacketSender);
    zoxd(NetAddress);
    zoxd(TargetNetAddress);
    zoxd_int(NetPort);
    zoxd_int(TargetNetPort);
    zoxd_arrayd(PacketData);
    zox_define_component_w_dest(SocketLink);
}