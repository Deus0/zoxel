#include "streamer.c"
// entity prefab_chunk_streaming;
entity prefab_streamer;

void prefab_add_terrain_camera(ecs *world, entity e) {
    zox_add_tag(e, Streamer);
    zox_prefab_set(e, StreamPoint, { int3_zero });
    zox_prefab_set(e, StreamPoint2, { int2_zero });
    zox_prefab_set(e, StreamLink, { 0 });
    zox_prefab_set(e, BlockScale, { 0 });
    zox_prefab_set(e, NodeDepth, { 0 });
}

void spawn_prefabs_streaming(ecs *world) {
    prefab_streamer = spawn_prefab_streamer(world);
    if (prefab_camera_game) {
        prefab_add_terrain_camera(world, prefab_camera_game);
    }
}
