#include "packet_send.c"
#include "packet_recieve.c"
#include "socket.c"

void define_systems_networking(ecs* world) {
    zox_system(
        PacketRecieveSystem,
        EcsOnUpdate,
        [in] SocketLink,
        [none] PacketReciever
    );
    zox_system(
        PacketSendSystem,
        EcsOnUpdate,
        [in] SocketLink,
        [in] TargetNetAddress,
        [in] TargetNetPort,
        [none] PacketSender
    );
    zox_system(
        SocketSystem,
        EcsOnUpdate,
        [in] NetPort,
        [out] SocketLink
    );
}