#include "sides.c"
#include "build.c"
#include "wait.c"
#include "meshes.c"
#include "trigger.c"
#include "toggle.c"

void define_systems_chunks3_textured(ecs *world) {
    zox_system(
        ChunkMeshTriggerSystem,
        zoxp_update,
        [in] chunks3.VoxelNodeDirty,
        // [out] chunks.BuildChunkSides,
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
        ChunkMeshToggleSystem,
        zoxp_update,
        [in] rendering.RenderDepth,
        [in] chunks3.ChunkLodDirty,
        [none] chunks3.ChunkTextured,
        [none] !chunks.GenerateChunk,
    );
    zox_system_1(
        ChunkMeshSpawnSystem,
        zoxp_mainthread,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [in] chunks3.VoxelNodeDirty,
        [none] chunks3.ChunkTextured
    );
    zox_system_1(
        ChunkMeshSpawn2System,
        zoxp_mainthread,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [out] chunks3.ChunkLodDirty,
        [none] chunks3.ChunkTextured
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
        [none] chunks3.ChunkTextured,
    );
    zox_system(
        ChunkTexturedBuildSystem,
        zoxp_voxels_mesh,
        [out] rendering.BuildMesh,
        [in] rendering.RenderDepth,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [out] rendering.MeshColorRGBs,
        [none] chunks.ChunkMesh,
        [none] !core.Disabled
    );
}
