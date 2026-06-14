#include "streamer.c"
entity prefab_streamer;

void zox_define_prefabs_streaming(ecs *world) {
    prefab_streamer = spawn_prefab_streamer(world);
    if (prefab_camera_game) {
        prefab_add_streamer(world, prefab_camera_game);
    }
}
