


int check_socket_error(char *debug) {
#ifdef zox_windows
    // Handle errors on Windows
    int error_code = WSAGetLastError();
    if (error_code != WSAEWOULDBLOCK) {
        if (zox_log_network_errors) {
            zox_loge("[%s] error_code %d", debug, error_code);
        }
    }
    return error_code;
#else
    // Handle errors on Unix-like systems
    if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
        if (zox_log_network_errors) {
            perror("    check_socket_error: recvfrom");
            zox_loge("[%s]", debug)
        }
    }
    return errno;
#endif
}

/*void set_new_socket(ecs *world, entity e, int port) {
    zox_set(e, SocketLink, { open_socket(port) })
}*/