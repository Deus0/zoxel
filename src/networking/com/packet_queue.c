#define zox_packet_queue_trigger 0
#define zox_packet_queue_process 1
#define zox_packet_queue_end 2

typedef struct {
    byte state;
    byte type;
    byte4 ip;
    uint port;
} PacketUpdate;

zoxc_queue(PacketQueue, PacketUpdate, 1);
zoxc_queue_remove_at(PacketQueue, PacketUpdate);
