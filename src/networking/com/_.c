zox_tag(Packet);
zox_tag(NetConnection);
zox_tag(NetRoom);
zox_tag(NetPlayer);
zox_tag(PacketReciever);
zox_tag(PacketSender);
zoxc_byte4(NetAddress);
zoxc_byte4(TargetNetAddress);
zoxc_int(NetPort);
zoxc_int(TargetNetPort);
zoxc_int(SocketLink);
zoxc_arrayd(PacketData, byte);
#include "socket_link.c"
#include "packet_queue.c"

void zoxd_components_networking(ecs* world) {
    zoxd_tag(Packet);
    zoxd_tag(NetConnection);
    zoxd_tag(NetRoom);
    zoxd_tag(NetPlayer);
    zoxd_tag(PacketReciever);
    zoxd_tag(PacketSender);
    zoxd(NetAddress);
    zoxd(TargetNetAddress);
    zoxd_int(NetPort);
    zoxd_int(TargetNetPort);
    zoxd_arrayd(PacketData);
    zoxd_dest(SocketLink);
    zoxd_queue(PacketQueue);
}
