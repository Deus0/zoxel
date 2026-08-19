#ifndef zox_windows
    int set_non_blocking(int sock) {
        int flags = fcntl(sock, f_getfl, 0);
        if (flags < 0) {
            perror("    open_socket: fcntl ");
            return 1;
        }
        if (fcntl(sock, f_setfl, flags | non_blocking) < 0) {
            perror("    open_socket: fcntl ");
            return 1;
        }
        return 0;
    }
#else

    int set_non_blocking(int sock) {
        // Set the socket to non-blocking mode
        u_long non_blocking = 1;
        if (ioctlsocket(sock, FIONBIO, &non_blocking) != 0) {
            perror(" ! open_socket: ioctlsocket");
            // closesocket(sock);
            return -1;
        }
        return 0;
    }
#endif

//! Returns socket integer, -1 if failed to bind socket.
int open_socket(int port) {
    // create the UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("    open_socket: socket ");
        return -1;
    }
    if (set_non_blocking(sock)) {
        return -1;
    }
    // set up the address to bind to
    struct sockaddr_in bind_addr;
    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;
    bind_addr.sin_port = htons(port);
    // bind the socket to the address
    if (bind(sock, (struct sockaddr*) &bind_addr, sizeof(bind_addr)) < 0) {
        perror("    open_socket: bind");
        return -1;
    }
    zox_log(" > success opening socket [%i] port [%i]", sock, port)
    return sock;
}

zox_sys2(SocketSystem) {
    byte dbg_log = 1;
    zox_sys_begin();
    zox_sys_in(NetPort);
    zox_sys_out(SocketLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(NetPort, port);
        zox_sys_o(SocketLink, socket);
        if (socket->value < 0) {
            if (dbg_log) {
                zox_log("Opening Socket Port [%i]", port->value);
            }
            socket->value = open_socket(port->value);
            if (dbg_log) {
                zox_log("+ Socket Opened [%i]", socket->value);
            }
        }
    }
} zox_sys_end(SocketSystem);