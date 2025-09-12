// atm using 127.0.0.1 for same machine testing
// later for secure packets, keep sending with udp every x seconds
#define server_ip (byte4) { 194, 195, 251, 84 }  //  "192.0.2.1"
#define local_server_ip (byte4) { 127, 0, 0, 1 }  //  "192.0.2.1"
#define PORT 12345
#define SERVER_PORT 12346
#define BUFFER_SIZE 128 // 1024 // the size of the buffer to use for receiving and sending data
#define ZOXEL_URL "https://zoxel.duckdns.org"
byte zox_is_networking = 0;
byte server_mode = 0;
#include "packet_types.c"

// Socket Settings
byte sockets_enabled = 0;

#ifndef zox_windows
    #define peek_packet_size 1
    #define socket_error_code -1
    const unsigned long non_blocking = O_NONBLOCK;
    const unsigned long f_getfl = F_GETFL;
    const unsigned long f_setfl = F_SETFL;
#else
    #define peek_packet_size 1024
    #define socket_error_code SOCKET_ERROR
    const unsigned long non_blocking = 1; // O_NONBLOCK
    const unsigned long f_getfl = 3; // F_GETFL
    const unsigned long f_setfl = 4; // F_SETFL
#endif

void process_arguments_networking(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "-x") == 0 || strcmp(args[i], "--server") == 0) {
            server_mode = 1;
        } else if (strcmp(args[i], "--client") == 0) {
            server_mode = 0;
        } else if (strcmp(args[i], "-x") == 0 || strcmp(args[i], "--server") == 0) {
            server_mode = 1;
        }
    }
}
