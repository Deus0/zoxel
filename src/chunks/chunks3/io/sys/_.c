#include "save.c"
#include "load.c"
#include "block_table.c"

void zox_systems_chunksio(ecs* world) {
    // main thread: reads and writes the realm's block table and remaps saved chunks (files)
    zox_system_1(
        block_table_system,
        zoxp_spawn,
        [in] realms.GenerateRealm,
        [in] blocks.BlockLinks,
        [none] realms.Realm,
        [none] blocks.BlockTableDirty
    );
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
