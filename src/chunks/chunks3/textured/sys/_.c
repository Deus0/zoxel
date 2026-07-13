#include "render.c"
#include "sides.c"
#include "build.c"
#include "wait.c"

void define_systems_chunks3_textured(ecs *world) {
    // move this into chunk3, for chunk3_textured
    zox_render3_system(0,
        Chunk3TexturedRenderSystem,
        [in] transforms.TransformMatrix,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.MesnRenderCount,
        [in] textures.TilemapLink,
        [in] rendering.RenderDisabled
    );
    zox_system(
        Chunk3SidesSystem,
        zoxp_update,
        [in] blocks.BlockManagerLink,
        [in] rendering.RenderDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] chunks3.BuildChunkMesh,
        [out] chunks3.SidesOctree,
        [out] chunks3.SidesOctreeDirty,
        [none] chunks3.ChunkTextured
    );
    // move this into chunk3, for chunk3_textured
    zox_system(
        Chunk3TexturedBuildSystem,
        zoxp_update,
        [in] blocks.BlockManagerLink,
        [in] textures.TilemapLink,
        [in] rendering.RenderDepth,
        [in] blocks.BlockScale,
        [in] chunks3.VoxelNode,
        [in] chunks3.SidesOctree,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [out] rendering.MeshColorRGBs,
        [out] chunks3.BuildChunkMesh,
        [out] rendering.MeshReady,
        [out] core.Busy,
        [none] chunks3.ChunkTextured
    );
    // Custom Debug
    zox_set(zox_id(Chunk3TexturedRenderSystem), SystemDeltaMax, {  zox_lag_cutoff * 2 });
    zox_system(
        ChunkMeshSlowSystem,
        zoxp_update,
        [out] rendering.MeshReady,
        [out] rendering.MeshDirty,
        [none] chunks3.ChunkTextured
    );
    add_system_process_counter(world, zox_id(Chunk3SidesSystem));
    add_system_process_counter(world, zox_id(Chunk3TexturedBuildSystem));
    add_system_process_counter(world, zox_id(Chunk3TexturedRenderSystem));
}
