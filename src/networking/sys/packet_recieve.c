// NOTE: Peeks at zox packet headers [1 byte]
static inline byte peek_at_packet(
    int socket,
    struct sockaddr_in *recv_addr,
    byte dbg_log)
{
    socklen_t recv_addr_len = sizeof(*recv_addr);
    byte recv_buffer[peek_packet_size];
    int recv_size = recvfrom(
        socket,
        (char *) recv_buffer,
        peek_packet_size,
        MSG_PEEK,
        (struct sockaddr*) recv_addr,
        &recv_addr_len);
    if (recv_size < 0) {
        check_socket_error("peek_at_packet");
        return 0;
    } else if (recv_size == 0) {
        return 0;
    } else {
        byte packet_type = recv_buffer[0];
        /*if (dbg_log) {
            zox_log("[Packet] Recieved [%s]",
                zox_get_packet_name(packet_type));
            // zox_log("     - sender ip [%s]\n", ip4_to_string(*recv_addr));
        }*/
        return packet_type;
    }
    return 0;
}

// NOTE: Consumes one zox packet byte.
static inline int consume_packet(
    int socket,
    byte *recv_buffer,
    struct sockaddr_in *recv_addr)
{
    socklen_t recv_addr_len = sizeof(*recv_addr);
    return recvfrom(
        socket,
        (char *) recv_buffer,
        1,
        0,
        (struct sockaddr*) recv_addr,
        &recv_addr_len);
}

zox_sys2(PacketRecieveSystem) {
    byte dbg_log = 1;
    uint zox_max_packet_reads = 256;
    byte recv_buffer[1];
    zox_sys_begin();
    zox_sys_in(SocketLink);
    zox_sys_out(PacketQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SocketLink, socket);
        zox_sys_o(PacketQueue, queue);
        if (socket->value < 0) {
            if (dbg_log >= 2) zox_log("Socket is disconnected...");
            continue;
        }
        if (dbg_log >= 2) zox_log("Hosting...");
        int recv_size;
        struct sockaddr_in recv_addr;
        socklen_t recv_addr_len = sizeof(recv_addr);
        for (uint j = 0; j < zox_max_packet_reads; j++) {
            byte consumed_packet = 0;
            byte packet_size = 1;
            byte packet_type = peek_at_packet(
                socket->value,
                &recv_addr,
                dbg_log);
            if (packet_type == 0) {
                if (dbg_log >= 2) zox_log("+ No Packets");
                break;  // No more packets to process
            }
            if (dbg_log >= 1) {
                zox_log("[Packet] Received [%s] from [%s]",
                    zox_get_packet_name(packet_type),
                    ip4_to_string(recv_addr));
            }
            if (packet_type == zox_packet_connect) {
                ushort port;
                byte4 ip = sockaddr_to_byte4(recv_addr, &port);
                a_PacketQueue(queue, (PacketUpdate) {
                    .type = packet_type,
                    .ip = ip,
                    .port = port });
                // check if client exists in children ip:port
                // if client doesnt exist, spawn them
                /*
                entity new_client = spawn_net_connection(
                    world,
                    recv_addr.ip,
                    recv_addr.port);
                    zox_set_parent(world, new_client, e);
                */
                // reply to client to confirm message was sent
                /*byte return_packet_size = 1;
                byte send_buffer[1] = { zox_packet_connect_confirm };   // return packet
                int send_size = sendto(
                    socket->value,
                    (const char *) send_buffer,
                    return_packet_size,
                    0,
                    (struct sockaddr*) &recv_addr,
                    sizeof(recv_addr));
                if (send_size == socket_error_code) {
                    check_socket_error("recieve_system");
                } else {
                    if (dbg_log) {
                        zox_log("[Packet] Sent return packet type [%i]",
                            send_buffer[0]);
                    }
                }*/
            } else if (packet_type == zox_packet_connect_confirm) {
                if (dbg_log) {
                    zox_log("[Packet] Client Connection Confirmed");
                }
            } else if (packet_type == zox_packet_msg_confirm) {
                if (dbg_log) {
                    zox_log("[Packet] Message Delivery Confirmed");
                }
            } else if (packet_type == zox_packet_msg) {
                // First peek at the complete packet header/body
                // so we can determine its actual size.
                byte recv_buffer_2[1 + peek_packet_size];
                recv_addr_len = sizeof(recv_addr);
                int text_size2 = recvfrom(
                    socket->value,
                    (char *) recv_buffer_2,
                    1 + peek_packet_size,
                    MSG_PEEK,
                    (struct sockaddr*) &recv_addr,
                    &recv_addr_len);
                if (text_size2 == socket_error_code) {
                    check_socket_error("recieve_system2");
                } else if (text_size2 < 2) {
                    // A message requires:
                    // [packet type][text length]
                    if (dbg_log) {
                        zox_log("[Packet] Invalid message packet size [%i]", text_size2);
                    }
                } else {
                    byte text_length = recv_buffer_2[1];
                    packet_size = 2 + text_length;
                    byte recv_buffer_3[packet_size];
                    recv_addr_len = sizeof(recv_addr);
                    recv_size = recvfrom(
                        socket->value,
                        (char *) recv_buffer_3,
                        packet_size,
                        0,
                        (struct sockaddr*) &recv_addr,
                        &recv_addr_len);
                    /*if (dbg_log) {
                        zox_log("[Packet] Server recieved message of size [%i]", text_length);
                    }*/
                    if (recv_size == socket_error_code) {
                        check_socket_error("recieve_system3");
                        // perror("    PacketRecieveSystem : zox_packet_msg 2 : recvfrom");
                    } else {
                        char* text = convert_to_ascii_text_at(recv_buffer_3, text_length, 2);
                        if (dbg_log) {
                            zox_log("[Packet] Message Recieved [%s]", text);
                        }
                        free(text);
                        consumed_packet = 1;
                    }
                }
            }
            // If a packet-specific branch did not consume the packet,
            // consume its first byte here.
            // This is mainly a safety fallback. Every currently-known
            // packet type consumes its packet above.
            if (!consumed_packet) {
                recv_size = consume_packet(
                    socket->value,
                    recv_buffer,
                    &recv_addr);
                if (recv_size == socket_error_code) {
                    check_socket_error("recieve_system_consume");
                    break;
                }
                consumed_packet = 1;
            }
        }
    }
} zox_sys_end(PacketRecieveSystem);