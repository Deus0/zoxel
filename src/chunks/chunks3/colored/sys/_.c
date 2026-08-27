#include "build.c"

void define_systems_chunks3_colored(ecs *world) {
    zox_system(
        ChunkColorsBuildSystem,
        zoxp_voxels_mesh,
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
        // [] rendering.MeshDirty,
        [none] chunks3.ColorChunk,
        [none] rendering.BuildMesh,
    );
    // zox_track_system(world, zox_id(ChunkColorsBuildSystem));
}
