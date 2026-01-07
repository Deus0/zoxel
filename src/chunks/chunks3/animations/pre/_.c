entity prefab_chunk_animating;
#include "animating_chunk.c"

void spawn_prefabs_voxel_animations(ecs *world) {
    prefab_chunk_animating = spawn_prefab_animating_chunk(world, prefab_chunk);
}