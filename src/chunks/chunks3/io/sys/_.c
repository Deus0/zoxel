#include "save.c"
#include "load.c"

void define_systems_chunksio(ecs* world) {
    zox_system(
        Chunk3SaveSystem,
        zoxp_save,
        [in] chunks3.VoxelNodeEdited,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkPosition,
        [none] chunks3.VoxelNodePostDirty,
    );
    zox_system(
        Chunk3LoadSystem,
        zoxp_load,
        [in] chunks3.ChunkPosition,
        [out] chunks.NodeDepth,
        [out] chunks3.VoxelNode,
        [out] saves.Loaded,
        [none] chunks3.Chunk3,
        [none] core.Initialize,
    );
}
