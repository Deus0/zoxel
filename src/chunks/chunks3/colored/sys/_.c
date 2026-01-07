#include "build.c"

void define_systems_chunks3_colored(ecs *world) {
    if (!headless) {
        zox_system(
            ChunkColorsBuildSystem,
            zoxp_voxels_read,
            [in] chunks3.ChunkMeshDirty,
            [in] chunks3.VoxelNode,
            [in] chunks3.NodeDepth,
            [in] rendering.RenderDepth,
            [in] chunks3.ChunkNeighbors,
            [in] colorz.ColorRGBs,
            [in] chunks3.ChunkSize,
            [in] blocks.BlockScale,
            [out] rendering.MeshIndicies,
            [out] rendering.MeshVertices,
            [out] rendering.MeshColorRGBs,
            [out] rendering.MeshDirty,
            [none] chunks3.ColorChunk
        );
    }
}