#include "save.c"
#include "load.c"

void define_systems_chunksio(ecs* world) {
    zox_system(
        Chunk3SaveSystem,
        EcsPreStore,
        [in] chunks3.VoxelNodeEdited,
        [in] chunks3.VoxelNodeDirty,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkPosition
    );
    zox_system(
        Chunk3LoadSystem,
        EcsOnLoad,
        [in] chunks3.ChunkPosition,
        [out] chunks3.VoxelNodeDirty,
        [out] chunks3.VoxelNodeEdited,
        [out] chunks3.VoxelNodeLoaded,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] chunks3.VoxelNodeGenerated,
    );
}