#include "packet_send.c"
#include "packet_recieve.c"
#include "socket.c"

void define_systems_networking(ecs* world) {
    zox_system(
        PacketRecieveSystem,
        zoxp_update,
        [in] SocketLink,
        [none] PacketReciever
    );
    zox_system(
        PacketSendSystem,
        zoxp_update,
        [in] SocketLink,
        [in] TargetNetAddress,
        [in] TargetNetPort,
        [none] PacketSender
    );
    zox_system(
        SocketSystem,
        zoxp_update,
        [in] NetPort,
        [out] SocketLink
    );
}