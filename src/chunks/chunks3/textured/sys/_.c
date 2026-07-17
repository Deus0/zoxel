#include "render.c"
#include "sides.c"
#include "build.c"
#include "wait.c"
#include "spawn_mesh.c"

void define_systems_chunks3_textured(ecs *world) {
    // move this into chunk3, for chunk3_textured
    zox_render3_system(
        0,
        Chunk3TexturedRenderSystem,
        [in] rendering.RenderDisabled,
        [in] transforms.TransformMatrix,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.MesnRenderCount,
        [none] chunks.ChunkMesh
    );
    zox_set(zox_id(Chunk3TexturedRenderSystem), SystemDeltaMax, {  zox_lag_cutoff * 2 });
    add_system_process_counter(world, zox_id(Chunk3TexturedRenderSystem));
    /*zox_system(
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
    add_system_process_counter(world, zox_id(Chunk3SidesSystem));*/
    zox_system(
        Chunk3Sides2System,
        zoxp_update,
        [in] rendering.RenderDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] chunks.BuildChunkSides,
        [out] chunks3.SidesOctree,
        [out] chunks3.SidesOctreeDirty,
        [none] chunks3.ChunkTextured
    );
    // move this into chunk3, for chunk3_textured
    /*zox_system(
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
    add_system_process_counter(world, zox_id(Chunk3TexturedBuildSystem));*/
    // Refactor Chunk Mesh
    zox_system(
        Chunk3TexturedBuild2System,
        zoxp_update,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [out] chunks3.BuildChunkMesh,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [out] rendering.MeshColorRGBs,
        [out] rendering.MeshReady,
        [none] chunks.ChunkMesh
    );
    zox_system(
        ChunkMeshSlowSystem,
        zoxp_update,
        [out] rendering.MeshReady,
        [out] rendering.MeshDirty,
        [none] chunks3.ChunkTextured
    );
    zox_system(
        ChunkMeshSlow2System,
        zoxp_update,
        [out] rendering.MeshReady,
        [out] rendering.MeshDirty,
        [none] chunks.ChunkMesh
    );
    zox_system_1(
        ChunkMeshSpawnSystem,
        zoxp_mainthread,
        [out] rendering.RenderDepth,
        [out] rendering.RenderDepthDirty,
        [none] chunks3.ChunkTextured
    );
    zox_system(
        Chunk3MeshTrigger2System,
        zoxp_update,
        [in] chunks3.VoxelNodeDirty,
        [out] chunks.BuildChunkSides,
        [none] chunks3.ChunkTextured
    );
    zox_system(
        Chunk3NeighborsMeshTriggerSystem,
        zoxp_update,
        [in] chunks3.VoxelNodeDirty,
        [in] chunks3.ChunkNeighbors,
        [none] chunks3.ChunkTextured
    );
}
