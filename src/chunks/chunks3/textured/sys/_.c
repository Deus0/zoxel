#include "render.c"
#include "sides.c"
#include "build.c"
#include "wait.c"
#include "meshes.c"
#include "trigger.c"

void define_systems_chunks3_textured(ecs *world) {
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
    zox_system(
        ChunkSidesSystem,
        zoxp_update,
        [in] rendering.RenderDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] chunks.BuildChunkSides,
        [out] chunks3.SidesOctree,
        [out] chunks3.SidesOctreeDirty,
        [none] chunks3.ChunkTextured
    );
    // Refactor Chunk Mesh
    zox_system(
        ChunkTexturedBuildSystem,
        zoxp_update,
        [in] rendering.RenderDepth,
        [out] chunks3.BuildChunkMesh,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [out] rendering.MeshColorRGBs,
        [out] rendering.TexturedMeshDirty,
        [none] chunks.ChunkMesh
    );
    zox_system(
        ChunkMeshToggleSystem,
        zoxp_update,
        [in] rendering.RenderDepth,
        [out] chunks3.ChunkLodDirty,
        [none] chunks3.ChunkTextured
    );
    zox_system_1(
        ChunkMeshSpawnSystem,
        zoxp_mainthread,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] chunks3.VoxelNodeDirty,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [out] chunks3.ChunkLodDirty,
        [none] chunks3.ChunkTextured
    );
    zox_render3_system(
        0,
        Chunk3TexturedRenderSystem,
        [in] rendering.RenderDisabled,
        [in] transforms.TransformMatrix,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.MeshRenderCount,
        [none] chunks.ChunkMesh
    );
    zox_set(zox_id(Chunk3TexturedRenderSystem), SystemDeltaMax, { zox_lag_cutoff * 2 });
    add_system_process_counter(world, zox_id(Chunk3TexturedRenderSystem));


    /*zox_system(
        ChunkMeshSlow2System,
        zoxp_update,
        [out] rendering.MeshReady,
        [out] rendering.TexturedMeshDirty,
        [none] chunks.ChunkMesh
    );*/
    /*zox_system(
        ChunkMeshSlowSystem,
        zoxp_update,
        [out] rendering.MeshReady,
        [out] rendering.TexturedMeshDirty,
        [none] chunks3.ChunkTextured
    );*/

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
}
