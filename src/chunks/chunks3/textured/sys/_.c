#include "sides.c"
#include "build.c"
#include "wait.c"
#include "spawn.c"
#include "trigger.c"
#include "transition.c"

void define_systems_chunks3_textured(ecs *world) {
    zox_system(
        ChunkSidesTriggerSystem,
        zoxp_update,
        [none] chunks3.VoxelNodePostDirty,
        [none] chunks.ChunkTextured
    );
    zox_system(
        ChunkNeighborsSidesTriggerSystem,
        zoxp_update,
        [in] chunks3.ChunkNeighbors,
        [none] chunks3.VoxelNodePostDirty,
        [none] chunks.ChunkTextured
    );
    // Builds our Terrain Chunk Mesh
    // NOTE: Requires reading voxel data
    zox_system(
        ChunkSidesSystem,
        zoxp_voxels_sides,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] chunks3.SidesOctree,
        [none] chunks.BuildChunkSides,
        [none] chunks.ChunkTextured,
        [none] !chunks3.VoxelNodeDirty,
    );
    zox_system_1(
        ChunkMeshSpawn2System,
        zoxp_spawn,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [out] chunks3.ChunkLodDirty,
        [out] chunks.ChunkMeshTimer,
        [none] chunks.ChunkTextured
    );
    zox_system_1(
        ChunkMeshSpawnSystem,
        zoxp_spawn,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [out] chunks.ChunkMeshTimer,
        [none] chunks3.VoxelNodePostDirty,
        [none] chunks.ChunkTextured
    );
    // hmmmm
    zox_system(
        ChunkMeshTransitionSystem,
        zoxp_update,
        [in] chunks3.ChunkLodDirty,
        [in] rendering.RenderDepth,
        [out] chunks.ChunkMeshTimer,
        [none] chunks.ChunkTextured,
        [none] !chunks.GenerateChunk,
        [none] !chunks.BuildChunkSides,
    );
    zox_system(
        ChunkTexturedBuildSystem,
        zoxp_voxels_mesh,
        [in] rendering.RenderDepth,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [out] rendering.MeshColorRGBs,
        [none] chunks.ChunkMesh,
        [none] rendering.BuildMesh,
        [none] !core.BuildDisabled,
    );
    /*zox_system_cached(
        ChunkTexturedBuildSystem2,
        zoxp_voxels_mesh,
        [none] rendering.RenderDepth,
        [none] chunks.ChunkMesh,
    );*/
}
