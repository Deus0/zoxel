zox_tag(Streamer);
zox_tag(StreamedChunk);
zoxc_byte(StreamerLevel);
zoxc_int2(StreamPosition2);
zoxc_int3(StreamPosition);
zox_tag(StreamLink);          // links a streamer to the terrain it streams
zoxc_state(StreamDirty);
zoxc_state(StreamDirty2);
zoxc_int(StreamRow);    // the next row of columns its terrain queues (centre first), -1 none (src/terrains/voxel/sys/stream.c)
zoxc_int3(StreamSwept); // the StreamPosition its last sweep was from: a sweep starts when they differ
zoxc_function(StreamEndEvent, void, ecs*, const entity);
#include "queue.c"

// its settings, each its own tag (a setting's observer finds it by it, not its name)
zox_tag(SettingNearRD);
zox_tag(SettingFarRD);
zox_tag(SettingVerticalRD);
zox_tag(SettingNoLods);
zox_tag(SettingNoFrustums);

void zox_components_streaming(ecs *world) {
    zoxd_tag(SettingNearRD);
    zoxd_tag(SettingFarRD);
    zoxd_tag(SettingVerticalRD);
    zoxd_tag(SettingNoLods);
    zoxd_tag(SettingNoFrustums);

    zoxd_tag(Streamer);
    zoxd_tag(StreamedChunk);
    zoxd_byte(StreamerLevel);
    zoxd_int2(StreamPosition2);
    zoxd_int3(StreamPosition);
    zoxd_nf_link(StreamLink);
    zoxd_state(StreamDirty);
    zoxd_state(StreamDirty2);
    zoxd_int(StreamRow);
    zoxd_int3(StreamSwept);
    zoxd(StreamEndEvent);
    // Queue
    define_terrain_spawn_queue(world);
}
