#include "cleanup.c"
#include "mesh_trigger.c"
#include "queue.c"
#include "clear.c"
#include "sides.c"
// voxes
#include "grow_bounds3.c"
#include "clone.c"
#include "combine.c"
#include "bake_vox.c"
#include "find_neighbor.c"
#include "block_scale.c"
#include "entities_lod.c"
#include "move_bounds.c"
#include "vox_texture.c"
// colored
#include "build_colored.c"
// textured
#include "build_textured.c"
#include "wait.c"
#include "mesh_spawn.c"
#include "trigger.c"
#include "transition.c"

void define_systems_chunks3(ecs *world) {
    zox_system(
        BakeVoxSystem,
        zoxp_update,
        [in] rendering.ModelLink,
        [in] textures.TextureLinks
    );
    zox_system(
        VoxTextureSystem,
        zoxp_update,
        [in] rendering.TextureSize,
        [in] blocks.VoxBakeSide,
        [in] rendering.ModelLink,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [none] textures.VoxTexture
    );
    // ColoredChunk, triggers mesh to update
    zox_system(
        ColoredChunkMeshTriggerSystem,
        zoxp_update,
        [none] chunks.Chunk,
        [none] chunks3.ColorChunk,
        [none] chunks3.VoxelNodePostDirty,
    );
    // NOTE: Writes to VoxelNode
    zox_system(
        CloneVoxSystem,
        zoxp_update,
        [in] CloneVoxLink,
        [out] CloneVox,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] blocks.BlockScale,
        [out] chunks3.ChunkSize,
        [out] colorz.ColorRGBs,
    );
    zox_system(
        CombineVoxSystem,
        zoxp_update,
        [in] chunks3.CombineVox,
        [in] chunks3.CombineList,
        [in] chunks3.CombinePositions,
        [out] chunks3.ChunkSize,
        [out] chunks.NodeDepth,
        [out] rendering.RenderDepth,
        [out] chunks3.VoxelNode,
        [out] colorz.ColorRGBs,
    );
    // Colored
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
        [none] chunks3.ColorChunk,
        [none] rendering.BuildMesh,
    );
    zox_system(
        ChunkFindNeighborSystem,
        zoxp_update,
        [in] chunks3.ChunkPosition,
        [out] chunks3.ChunkNeighbors,
        [none] chunks.ChunkTextured,
        [none] chunks.FindNeighbors,
    );
    // NOTE: Syncs Terrain Chunk Scales
    zox_system(
        BlockScaleSystem,
        zoxp_update,
        [in] rendering.RenderDepthDirty,
        [in] rendering.RenderDepth,
        [out] blocks.BlockScale
    );
    zox_system(
        ChunkEntitiesLodSystem,
        zoxp_update,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDistance,
        [in] chunks3.ChunkEntities
    );
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
        [none] chunks.ChunkTextured,
        [none] chunks.BuildChunkSides,
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
        [out] chunks.ChunkMeshTimer,
        // [out] rendering.ActiveMesh,
        // [out] rendering.PreparingMesh,
        [none] chunks.ChunkTextured,
        [none] !chunks.GenerateChunk,
        [none] !chunks.BuildChunkSides,
    );
    zox_system(
        VoxelOctreeOptimizeSystem,
        zoxp_remove,
        [out] chunks3.VoxelNodeLock,
        [out] chunks3.VoxelNode,
        [none] chunks.Chunk,
        [none] chunks3.VoxelNodeDirty,
    );
    zox_system(
        VoxelNodePostDirtySystem,
        zoxp_remove,
        [none] chunks.Chunk,
        [none] chunks3.VoxelNodePostDirty,
    );
    zox_system(
        VoxelUpdateQueueSystem,
        zoxp_queue_pre_clear,
        [in] chunks.NodeDepth,
        [out] chunks3.VoxelNodeQueue,
        [out] chunks3.VoxelNode,
        [none] chunks.Chunk,
    );
    zox_system(
        chunk_textured_build_system,
        zoxp_update, // zoxp_voxels_mesh zoxp_update
        [in] rendering.RenderDepth,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [out] rendering.MeshColorRGBs,
        [none] chunks.ChunkMesh,
        [none] rendering.BuildMesh,
        [none] !core.BuildDisabled,
    );
}
