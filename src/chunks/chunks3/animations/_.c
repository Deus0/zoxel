const double animating_chunk_speed = 0.5;
zoxc_double(AnimateChunk);
#include "pre/_.c"
#include "sys/_.c"

void import_chunks3_animations(ecs* world) {
    zox_module(chunks3_animations);
    zoxd_double(AnimateChunk);
    zox_systems_chunks3_noise(world);
    add_hook_spawn_prefabs(spawn_prefabs_voxel_animations);
}
