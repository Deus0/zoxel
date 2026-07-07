#include "animate_chunk_system.c"
#include "noise.c"

void zox_define_systems_chunks3_noise(ecs* world) {
    zox_system(
        NoiseVoxelNodeSystem,
        EcsOnUpdate,
        [in] core.Generate,
        [in] chunks.NodeDepth,
        [out] chunks3.VoxelNode,
        [out] colorz.ColorRGBs,
        [out] chunks3.BuildChunkMesh,
        [none] chunks3.NoiseChunk
    );
}
