#include "packet_send.c"
#include "packet_recieve.c"
#include "socket.c"
#include "queue.c"
#include "connect.c"

void zoxd_systems_networking(ecs* world) {
    zox_system(
        socket_system,
        zoxp_update,
        [in] networking.NetPort,
        [out] networking.SocketLink
    );
    zox_system(
        packet_recieve_system,
        zoxp_update,
        [in] networking.SocketLink,
        [out] networking.PacketQueue,
        [none] networking.PacketReciever,
    );
    zox_system(
        packet_queue_system,
        zoxp_update,
        [out] PacketQueue,
    );
    zox_system_1(
        net_connect_system,
        zoxp_spawn, // zoxp_update,
        [in] networking.SocketLink,
        [in] networking.PacketQueue,
        [none] networking.PacketReciever,
    );
    // Testing
    zox_system(
        fake_packet_send_system,
        zoxp_update,
        [in] networking.TargetNetAddress,
        [in] networking.TargetNetPort,
        [none] networking.PacketSender
    );
}