#include "save.c"
#include "load.c"

void define_systems_chunksio(ecs* world) {
    zox_system(
        Chunk3SaveSystem,
        EcsPreStore,
        [in] realms.RealmLink,
        [in] chunks3.VoxelNodeEdited,
        [in] chunks3.VoxelNodeDirty,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkPosition
    );
    zox_system(
        Chunk3LoadSystem,
        EcsOnLoad,
        [in] core.EntityInitialize,
        [in] realms.RealmLink,
        [in] chunks3.ChunkPosition,
        [out] chunks.NodeDepth,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [out] core.Loaded,
        [none] chunks3.Chunk3
        // [out] chunks3.VoxelNodeGenerated,
    );
}
