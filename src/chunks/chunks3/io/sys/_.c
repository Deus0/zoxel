#include "save.c"
#include "load.c"

void define_systems_chunksio(ecs* world) {
    zox_system(
        chunk3_load_system,
        zoxp_load,
        [in] chunks3.ChunkPosition,
        [out] chunks.NodeDepth,
        [out] chunks3.VoxelNode,
        [none] chunks.ChunkTextured,
        [none] saves.Saver,
        [none] core.Initialize,
    );
    zox_system(
        chunk3_save_system,
        zoxp_save,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkPosition,
        [out] chunks3.VoxelNodeLock,
        [none] chunks.ChunkTextured,
        [none] saves.Saver,
        [none] saves.Edited,
        [none] chunks3.ChunkUpdate,
    );
}
