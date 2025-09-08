zox_tag(Streamer);
zox_tag(StreamedChunk);
zoxc_int3(StreamPoint);
zoxc_byte(StreamDirty);
zoxc_function(StreamEndEvent, void, ecs*, const entity);

void define_components_streaming(ecs *world) {
    zoxd_tag(Streamer);
    zoxd_tag(StreamedChunk);
    zoxd_int3(StreamPoint);
    zoxd_byte(StreamDirty);
    zoxd(StreamEndEvent);
}