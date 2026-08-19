#include "ip.c"
#include "socket.c"
#include "packet_test.c"

void process_arguments_networking(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "-x") == 0 || strcmp(args[i], "--server") == 0) {
            server_mode = 1;
            zox_log(" + Enabled [server]");
        } else if (strcmp(args[i], "--client") == 0) {
            server_mode = 0;
            zox_log(" + Enabled [client]");
        } else if (strcmp(args[i], "-x") == 0 || strcmp(args[i], "--server") == 0) {
            server_mode = 1;
        } else if (strcmp(args[i], "--offline") == 0) {
            zox_is_networking = 0;
            zox_log(" + Enabled [offline]");
        }  else if (strcmp(args[i], "--online") == 0) {
            zox_is_networking = 1;
            zox_log(" + Enabled [online]");
        } else if (strcmp(args[i], "--lan") == 0) {
            server_ip = local_ip;
            zox_log(" + Enabled [lan]");
        }
    }
}

#ifdef zox_windows
void initialize_windows_sockets() {
    if (sockets_enabled) {
        return;
    }
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        int error_code = WSAGetLastError();
        zox_log("    open_socket: WSAStartup failed with error code %d\n", error_code)
    } else {
        sockets_enabled = 1;
        zox_log(" + enabled windows sockets")
    }
}

void dispose_windows_sockets() {
    if (!sockets_enabled) return;
    WSACleanup();
}
#endif

void on_boot_networking(ecs* world, entity app) {
    if (!zox_is_networking) {
        return;
    }
#ifdef zox_windows
    initialize_windows_sockets();
#else
    sockets_enabled = 1;
#endif
    if (server_mode) {
        spawn_net_room(
            world,
            SERVER_PORT);
        zox_log("Zox Server - Port [%i]", SERVER_PORT);
    } else {
        spawn_net_player(
            world,
            PORT,
            server_ip,
            SERVER_PORT);
        zox_log("Zox Client - Port [%i]", PORT);
        zox_log("   -> Server At [%ix%ix%ix%i:%i]", server_ip.x, server_ip.y, server_ip.z, server_ip.w, SERVER_PORT);
    }
}