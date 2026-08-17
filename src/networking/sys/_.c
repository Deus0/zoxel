#include "packet_send.c"
#include "packet_recieve.c"
#include "socket.c"
#include "queue.c"
#include "connect.c"

void zoxd_systems_networking(ecs* world) {
    zox_system(
        SocketSystem,
        zoxp_update,
        [in] networking.NetPort,
        [out] networking.SocketLink
    );
    zox_system(
        PacketRecieveSystem,
        zoxp_update,
        [in] networking.SocketLink,
        [out] networking.PacketQueue,
        [none] networking.PacketReciever,
    );
    zox_system(
        PacketQueueSystem,
        zoxp_update,
        [out] PacketQueue,
    );
    zox_system_1(
        NetConnectSystem,
        zoxp_mainthread, // zoxp_update,
        [in] networking.SocketLink,
        [in] networking.PacketQueue,
        [none] networking.PacketReciever,
    );
    // Testing
    zox_system(
        FakePacketSendSystem,
        zoxp_update,
        [in] networking.TargetNetAddress,
        [in] networking.TargetNetPort,
        [none] networking.PacketSender
    );
}