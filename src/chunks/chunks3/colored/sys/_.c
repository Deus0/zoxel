#include "build.c"

void define_systems_chunks3_colored(ecs *world) {
    zox_system(
        ChunkColorsBuildSystem,
        EcsOnUpdate,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [in] rendering.RenderDepth,
        [in] chunks3.ChunkNeighbors,
        [in] colorz.ColorRGBs,
        [in] chunks3.ChunkSize,
        [in] blocks.BlockScale,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshColorRGBs,
        [out] chunks3.BuildChunkMesh,
        [out] rendering.MeshDirty,
        // [out] core.Busy,
        [none] chunks3.ColorChunk
    );
    // zox_track_system(world, zox_id(ChunkColorsBuildSystem));
}
