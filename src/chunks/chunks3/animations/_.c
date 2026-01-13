#ifndef zox_voxels_animations
#define zox_voxels_animations

const double animating_chunk_speed = 0.5;
zoxc_double(AnimateChunk);
#include "pre/_.c"
#include "sys/animate_chunk_system.c"
#include "sys/noise_chunk_system.c"

zox_begin_module(VoxelsAnimations)
    add_hook_spawn_prefabs(spawn_prefabs_voxel_animations);
    zoxd_double(AnimateChunk);
    zox_filter(
        noise_chunks,
        [none] chunks3.NoiseChunk,
        [out] chunks3.GenerateChunk
    );
    zox_system(
        AnimateChunkSystem,
        EcsOnUpdate,
        [out] AnimateChunk,
        [out] chunks3.GenerateChunk
    );
    // NOTE: Writes to VoxelNode
    zox_system_ctx(
        NoiseVoxelNodeSystem,
        EcsPreUpdate,
        noise_chunks,
        [in] chunks.NodeDepth,
        [out] chunks3.ChunkDirty,
        [out] chunks3.VoxelNode,
        [out] chunks3.GenerateChunk,
        [none] chunks3.NoiseChunk
    );
zox_end_module(VoxelsAnimations)

#endif