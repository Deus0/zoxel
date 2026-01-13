#include "chunk2.c"
entity prefab_chunk2;

void spawn_prefabs_chunks2(ecs *world) {
    prefab_chunk2 = spawn_prefab_chunk2(world);
}