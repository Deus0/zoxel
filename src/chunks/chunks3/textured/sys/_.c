#include "render.c"
#include "sides.c"
#include "build.c"
#include "build_high.c"

void define_systems_chunks3_textured(ecs *world) {

    if (headless) {
        return;
    }

    // move this into chunk3, for chunk3_textured
    zox_render3D_system(
        Chunk3TexturedRenderSystem,
        [in] transforms.TransformMatrix,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.MeshIndiciesGpu,
        [in] textures.TilemapLink,
        [in] rendering.RenderDisabled
    );

    zox_system(
        Chunk3SidesSystem,
        EcsPreUpdate,
        [in] blocks.BlockManagerLink,
        [in] chunks3.ChunkMeshDirty,
        [in] rendering.RenderDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] chunks3.SidesOctree,
        [out] chunks3.SidesOctreeDirty,
        [none] chunks3.ChunkTextured
    );

    // move this into chunk3, for chunk3_textured
    zox_system(
        Chunk3TexturedBuildSystem,
        EcsOnUpdate,
        [in] blocks.BlockManagerLink,
        [in] textures.TilemapLink,
        [in] chunks3.ChunkMeshDirty,
        [in] rendering.RenderDepth,
        // [in] chunks3.ChunkNeighbors,
        [in] blocks.BlockScale,
        [in] chunks3.VoxelNode,
        [in] chunks3.SidesOctree,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [out] rendering.MeshColorRGBs,
        [out] rendering.MeshDirty,
        [none] chunks3.ChunkTextured
    );

    /*zox_system(
        Chunk3TexturedHighBuildSystem,
        zoxp_voxels_read,
        [in] blocks.BlockManagerLink,
        [in] textures.TilemapLink,
        [in] chunks3.ChunkMeshDirty,
        [in] chunks3.VoxelNode,
        [in] rendering.RenderDepth,
        [in] chunks3.ChunkNeighbors,
        [in] blocks.BlockScale,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [out] rendering.MeshColorRGBs,
        [out] rendering.MeshDirty,
        [none] chunks3.ChunkTextured
    );*/


    // Custom Debug
    zox_set(zox_id(Chunk3TexturedRenderSystem), SystemDeltaMax, { 6 });
}
