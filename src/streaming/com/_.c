zox_tag(Streamer);
zox_tag(StreamedChunk);
zoxc_byte(StreamerLevel);
zoxc_int2(StreamPosition2);
zoxc_int3(StreamPosition);
zox_tag(StreamLink);          // links a streamer to the terrain it streams
zoxc_state(StreamDirty);
zoxc_state(StreamDirty2);
zoxc_function(StreamEndEvent, void, ecs*, const entity);
#include "queue.c"

void zox_components_streaming(ecs *world) {
    zoxd_tag(Streamer);
    zoxd_tag(StreamedChunk);
    zoxd_byte(StreamerLevel);
    zoxd_int2(StreamPosition2);
    zoxd_int3(StreamPosition);
    zoxd_nf_link(StreamLink);
    zoxd_state(StreamDirty);
    zoxd_state(StreamDirty2);
    zoxd(StreamEndEvent);
    // Queue
    define_terrain_spawn_queue(world);
}
