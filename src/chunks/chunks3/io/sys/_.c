#include "save.c"
#include "load.c"

void define_systems_chunksio(ecs* world) {
    zox_system(
        Chunk3SaveSystem,
        zoxp_update,
        [in] chunks3.VoxelNodeEdited,
        [in] chunks3.VoxelNodeDirty,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkPosition
    );
    zox_system(
        Chunk3LoadSystem,
        zoxp_update,
        [in] core.Initialize,
        [in] chunks3.ChunkPosition,
        [out] chunks.NodeDepth,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [out] saves.Loaded,
        [none] chunks3.Chunk3
    );
}
