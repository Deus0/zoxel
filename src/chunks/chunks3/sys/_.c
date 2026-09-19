#include "states.c"
#include "optimize.c"
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

void zox_systems_chunks3(ecs *world) {
    zox_system(
        BakeVoxSystem,
        zoxp_update,
        [in] rendering.ModelLink,
        // [in] textures.TextureLinks,
        // [none] blocks.Block,
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
        [none] chunks3.Chunk3,
        [none] chunks3.ColorChunk,
        [none] chunks3.ChunkUpdate,
        [none] !rendering.BuildMesh,
    );
    // NOTE: Writes to VoxelNode
    zox_system(
        CloneVoxSystem,
        zoxp_octree_write,
        [in] CloneVoxLink,
        [out] CloneVox,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] blocks.BlockScale,
        [out] chunks3.ChunkSize,
        [out] colorz.ColorRGBs,
        [none] chunks3.Chunk3,
    );
    zox_system(
        CombineVoxSystem,
        zoxp_octree_write,
        [in] chunks3.CombineVox,
        [in] chunks3.CombineList,
        [in] chunks3.CombinePositions,
        [out] chunks3.ChunkSize,
        [out] chunks.NodeDepth,
        [out] rendering.RenderDepth,
        [out] chunks3.VoxelNode,
        [out] colorz.ColorRGBs,
        [none] chunks3.Chunk3,
    );
    // Colored
    zox_system(
        ChunkColorsBuildSystem,
        zoxp_octree_read,
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
        chunk_neighbors_system,
        zoxp_update,
        [in] chunks3.ChunkPosition,
        [out] chunks3.ChunkNeighbors,
        [none] chunks3.Chunk3,
        [none] chunks.FindNeighbors,
    );
    // NOTE: Syncs Terrain Chunk Scales
    zox_system(
        BlockScaleSystem,
        zoxp_update,
        [in] rendering.RenderDepthDirty,
        [in] rendering.RenderDepth,
        [out] blocks.BlockScale,
        [none] chunks3.Chunk3,
    );
    zox_system(
        ChunkEntitiesLodSystem,
        zoxp_update,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDistance,
        [in] chunks3.ChunkEntities,
        [none] chunks3.Chunk3,
    );
    zox_system(
        ChunkSidesTriggerSystem,
        zoxp_update,
        [none] chunks3.Chunk3,
        [none] chunks3.ChunkUpdate,
        // [none] chunks.ChunkTextured,
    );
    zox_system(
        ChunkNeighborsSidesTriggerSystem,
        zoxp_update,
        [in] chunks3.ChunkNeighbors,
        [none] chunks3.ChunkUpdate,
        [none] chunks.ChunkTextured
    );
    // Builds our Terrain Chunk Mesh
    // NOTE: Requires reading voxel data
    zox_system(
        chunk_sides_system,
        zoxp_octree_read,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeLock,
        [out] chunks3.SidesOctree,
        [out] chunks3.SidesOctreeLock,
        [none] chunks.ChunkTextured,
        [none] chunks.BuildChunkSides,
    );
    // Spawn mesh when voxel updates
    zox_system_1(
        ChunkMeshSpawnSystem,
        zoxp_spawn,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeLock,
        [out] chunks.ChunkMeshTimer,
        [none] chunks3.ChunkUpdate,
        [none] chunks.ChunkTextured,
    );
    zox_system_1(
        ChunkMeshSpawn2System,
        zoxp_spawn,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeLock,
        [out] chunks3.ChunkLodDirty,
        [out] chunks.ChunkMeshTimer,
        [none] chunks.ChunkTextured,
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
        voxel_octree_optimize_system,
        zoxp_octree_write,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeLock,
        [none] chunks3.Chunk3,
        [none] chunks3.VoxelNodePostDirty,
    );
    zox_system(
        voxel_node_dirty_system,
        zoxp_remove,
        [none] chunks3.Chunk3,
        [none] chunks3.VoxelNodeDirty,
    );
    zox_system(
        post_voxel_node_dirty_system,
        zoxp_remove,
        [none] chunks3.Chunk3,
        [none] chunks3.VoxelNodePostDirty,
    );
    zox_system(
        voxel_node_update_system,
        zoxp_remove,
        [none] chunks3.Chunk3,
        [none] chunks3.ChunkUpdate,
    );
    zox_system(
        VoxelUpdateQueueSystem,
        zoxp_octree_write,
        [in] chunks.NodeDepth,
        [out] chunks3.VoxelNodeQueue,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeLock,
        [none] chunks3.Chunk3,
    );
    zox_system(
        chunk_textured_build_system,
        zoxp_octree_read,
        [in] rendering.RenderDepth,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshUVs,
        [out] rendering.MeshColorRGBs,
        [none] rendering.ChunkMesh,
        [none] rendering.BuildMesh,
        [none] !core.BuildDisabled,
    );
}
