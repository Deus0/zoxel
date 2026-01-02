#include "streamer.c"
// entity prefab_chunk_streaming;
entity prefab_streamer;

void prefab_add_terrain_camera(ecs *world, const entity e) {
    zox_add_tag(e, Streamer);
    zox_prefab_set(e, StreamPoint, { int3_zero });
    zox_prefab_set(e, TerrainLink, { 0 });
    zox_prefab_set(e, TerrainLink, { 0 });
}

void spawn_prefabs_streaming(ecs *world) {
    prefab_streamer = spawn_prefab_streamer(world);
    if (prefab_terrain) {
        zox_prefab_set(prefab_terrain, StreamEndEvent, { NULL });
    }
    if (prefab_chunk_terrain) {
        zox_add_tag(prefab_chunk_terrain, StreamedChunk);
    }
    if (prefab_camera_game) {
        prefab_add_terrain_camera(world, prefab_camera_game);
    }
}
