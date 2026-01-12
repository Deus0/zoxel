zox_sys2(SocketSystem) {
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
} zox_sys_end(SocketSystem);