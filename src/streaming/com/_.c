zox_tag(Streamer);
zox_tag(StreamedChunk);
zoxc_int2(StreamPoint2);
zoxc_int3(StreamPoint);
zoxc_entity(StreamLink);
zoxc_state(StreamDirty);
zoxc_function(StreamEndEvent, void, ecs*, const entity);

void define_components_streaming(ecs *world) {
    zoxd_tag(Streamer);
    zoxd_tag(StreamedChunk);
    zoxd_int2(StreamPoint2);
    zoxd_int3(StreamPoint);
    zoxd_entity(StreamLink);
    zoxd_state(StreamDirty);
    zoxd(StreamEndEvent);
}