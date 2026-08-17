#define zox_packet_connect         1
#define zox_packet_connect_confirm 2
#define zox_packet_reconnect       3
#define zox_packet_msg             4
#define zox_packet_msg_confirm     5
#define zox_packet_end             6

const char* packet_type_names[] = {
    "None",
    "Connect",
    "Connect Confirm",
    "Reconnect",
    "Message",
    "Message Confirm"
};

static inline const char* zox_get_packet_name(byte type) {
    if (type >= zox_packet_end) {
        zox_loge("Packet Type Oob [%i]", type);
        return "Unknown";
    }
    return packet_type_names[type];
}