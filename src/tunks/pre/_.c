#include "tunk.c"
entity prefab_tunk2;

void spawn_prefabs_tunks(ecs* world) {
    prefab_tunk2 = spawn_prefab_tunk(world, prefab_chunk2);
}
