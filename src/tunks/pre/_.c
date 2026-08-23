#include "tunk.c"
entity prefab_tunk2;

void spawn_prefabs_tunks(ecs* world) {
    prefab_tunk2 = spawn_prefab_tunk2(world, prefab_chunk2);
    if (prefab_camera_game) {
        zox_set(prefab_camera_game, TunkLink, { 0 });
    }
}
