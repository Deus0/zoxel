zox_tag(Streamer);
zox_tag(StreamedChunk);
zoxc_byte(StreamerLevel);
zoxc_int2(StreamPosition2);
zoxc_int3(StreamPosition);
zoxc_entity(StreamLink);
zoxc_state(StreamDirty);
zoxc_state(StreamDirty2);
zoxc_function(StreamEndEvent, void, ecs*, const entity);
#include "queue.c"

void define_components_streaming(ecs *world) {
    zoxd_tag(Streamer);
    zoxd_tag(StreamedChunk);
    zoxd_byte(StreamerLevel);
    zoxd_int2(StreamPosition2);
    zoxd_int3(StreamPosition);
    zoxd_entity(StreamLink);
    zoxd_state(StreamDirty);
    zoxd_state(StreamDirty2);
    zoxd(StreamEndEvent);
    // Queue
    define_terrain_spawn_queue(world);
}
