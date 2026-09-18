#ifndef zox_voxels_animations
#define zox_voxels_animations

const double animating_chunk_speed = 0.5;
zoxc_double(AnimateChunk);
#include "pre/_.c"
#include "sys/_.c"

zox_begin_module(VoxelsAnimations) {
    zoxd_double(AnimateChunk);
    zox_systems_chunks3_noise(world);
    add_hook_spawn_prefabs(spawn_prefabs_voxel_animations);
} zox_end_module(VoxelsAnimations)

#endif
