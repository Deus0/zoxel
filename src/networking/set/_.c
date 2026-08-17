// atm using 127.0.0.1 for same machine testing
// later for secure packets, keep sending with udp every x seconds
#define ZOXEL_URL "https://zoxel.duckdns.org"
#define PORT 12345
#define SERVER_PORT 12346
#define BUFFER_SIZE 128 // 1024 // the size of the buffer to use for receiving and sending data
byte zox_is_networking = 0; // NOTE: Disabled until its all working
byte zox_log_network_errors = 0;
// Socket Settings
byte sockets_enabled = 0;
#ifdef zox_server
    byte server_mode = 1;
#else
    byte server_mode = 0;
#endif
byte4 server_ip = (byte4) { 194, 195, 251, 84 };  //  "192.0.2.1"
byte4 local_ip = (byte4) { 127, 0, 0, 1 };  //  "192.0.2.1"

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
