void SocketSystem(iter *it) {
    zox_sys_begin();
    zox_sys_in(NetPort);
    zox_sys_out(SocketLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(NetPort, port);
        zox_sys_o(SocketLink, socket);

        if (socket->value) {
            continue;
        }

        socket->value = open_socket(port->value);
    }
} zoxd_system2(SocketSystem);