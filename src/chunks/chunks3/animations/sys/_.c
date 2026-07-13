#include "animate_chunk_system.c"
#include "noise.c"

void zox_define_systems_chunks3_noise(ecs* world) {
    zox_system(
        NoiseVoxelNodeSystem,
        zoxp_update,
        [in] chunks.NodeDepth,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] colorz.ColorRGBs,
        [out] chunks3.BuildChunkMesh,
        [none] chunks3.NoiseChunk
    );
}
