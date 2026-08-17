double last_time_send = 0;
int test_send_packet_length = 1; // 100;    // 3

// NOTE: Sends one zox packet to a target address.
static inline int send_packet(
    int socket,
    const byte *packet,
    int packet_size,
    struct sockaddr_in *target_addr)
{
    return sendto(
        socket,
        (const char *) packet,
        packet_size,
        0,
        (struct sockaddr*) target_addr,
        sizeof(*target_addr));
}

//! Atm this is just testing sending to server and getting replies
// NOTE: Testing: Every 3 seconds send one packet!
zox_sys2(FakePacketSendSystem) {
    byte dbg_log = 1;
    last_time_send += zox_delta_time;
    if (last_time_send < 3.0) {
        return;
    }
    last_time_send -= 3.0;
    const char *send_text = "Hello World.";
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TargetNetAddress);
    zox_sys_in(TargetNetPort);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TargetNetAddress, target);
        zox_sys_i(TargetNetPort, target_port);
        entity parent = zox_get_parent(world, e);
        if (!zox_valid(parent) || !zox_has(parent, SocketLink)) {
            continue;
        }
        const SocketLink* socket = zox_get(parent, SocketLink);
        if (socket->value < 0) {
            continue;
        }
        struct sockaddr_in send_addr = byte4_to_ip(target->value, target_port->value);
        byte packet_size = 1;
        byte send_buffer[] = { zox_packet_connect };
        byte *send_buffer_2 = convert_from_ascii_text_at(send_text, 2);
        send_buffer_2[0] = zox_packet_msg;
        send_buffer_2[1] = strlen(send_text);
        byte packet_size_2 = 2 + send_buffer_2[1];
        byte packets_send_count = 1 + rand() % test_send_packet_length;
        if (dbg_log >= 2) {
            zox_log("[Packet] Sending [%i] packets to [%i.%i.%i.%i:%i]",
                packets_send_count,
                target->value.x,
                target->value.y,
                target->value.z,
                target->value.w,
                target_port->value);
        }
        // test sending multiple times
        for (int j = 0; j < packets_send_count; j++) {
            if (rand() % 100 >= 70) {
                // send text instead
                int send_size = sendto(
                    socket->value,
                    (const char *) send_buffer_2,
                    packet_size_2,
                    0,
                    (struct sockaddr*) &send_addr,
                    sizeof(send_addr));
                if (send_size == socket_error_code) {
                    check_socket_error("send_system");
                } else {
                    if (dbg_log) {
                        zox_log("[Packet] Sent [%s]: [%s]",
                            zox_get_packet_name(send_buffer_2[0]),
                            send_text);
                    }
                }
            } else {
                int send_size = sendto(
                    socket->value,
                    (const char *) send_buffer,
                    packet_size,
                    0, (struct sockaddr*) &send_addr,
                    sizeof(send_addr));
                if (send_size == socket_error_code) {
                    check_socket_error("send_system2");
                } else {
                    if (dbg_log) {
                        zox_log("[Packet] Sent [%s]",
                            zox_get_packet_name(send_buffer[0]));
                    }
                }
            }
        }
        free(send_buffer_2);
    }
} zox_sys_end(FakePacketSendSystem);