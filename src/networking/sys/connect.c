zox_sys2(NetConnectSystem) {
    byte dbg_log = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SocketLink);
    zox_sys_in(PacketQueue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SocketLink, socket);
        zox_sys_i(PacketQueue, queue);
        if (socket->value < 0) {
            continue;
        }
        for (int j = queue->count - 1; j >= 0; j--) {
            const PacketUpdate* update = &queue->ptr[j];
            if (!(update->type == zox_packet_connect &&
                update->state == zox_packet_queue_process)) {
                continue;
            }
            struct sockaddr_in send_address = byte4_to_ip(update->ip, update->port);
            if (dbg_log) {
                zox_log("Net Client Connecting [%s]: %s", ip4_to_string(send_address), zox_get_packet_name(update->type));
            }
            entity old_client = 0;
            iter it2 = zox_children(world, e);
            while (zox_children_next(it2)) {
                for (int i = 0; i < it2.count; i++) {
                    entity e2 = it2.entities[i];
                    if (!zox_has(e2, TargetNetAddress)) {
                        continue;
                    }
                    byte4 ip = zox_getv(e2, TargetNetAddress);
                    ushort port = zox_getv(e2, TargetNetPort);
                    if (byte4_equals(ip, update->ip) && port == update->port) {
                        old_client = e2;
                        break;
                    }
                }
                if (old_client) {
                    break;
                }
            }
            byte reply = old_client ?
                zox_packet_reconnect :
                zox_packet_connect_confirm;
            if (!old_client) {
                // if doesnt exist
                entity new_client = spawn_net_connection(
                    world,
                    update->ip,
                    update->port);
                zox_set_parent(world, new_client, e);
            }
            // return packet
            byte return_packet_size = 1;
            byte send_buffer[1] = { reply };
            // TODO: Make a helper for sending
            int send_size = sendto(
                socket->value,
                (const char *) send_buffer,
                return_packet_size,
                0,
                (struct sockaddr*) &send_address,
                sizeof(send_address));
            if (send_size == socket_error_code) {
                check_socket_error("recieve_system");
            } else {
                if (dbg_log) {
                    zox_log("[Packet] Returned [%s]",
                        zox_get_packet_name(update->type));
                }
            }
        }
    }
} zox_sys_end(NetConnectSystem);