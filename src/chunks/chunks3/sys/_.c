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
#include "vox_lods.c"
// textured
#include "build_textured.c"
#include "wait.c"
#include "mesh_spawn.c"
#include "trigger.c"
#include "transition.c"

void zox_systems_chunks3(ecs *world) {
    zox_system(
        bake_vox_system,
        zoxp_update,
        // [in] textures.TextureLinks,
        [none] blocks.Block,
    );
    zox_system(
        vox_texture_system,
        zoxp_update,
        [in] rendering.TextureSize,
        [in] blocks.VoxBakeSide,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [none] textures.VoxTexture
    );
    // ColoredChunk, triggers mesh to update
    zox_system(
        colored_chunk_mesh_trigger_system,
        zoxp_update,
        [none] chunks3.Chunk3,
        [none] chunks3.ColorChunk,
        [none] chunks3.ChunkUpdate,
        [none] !rendering.BuildMesh,
    );
    // VoxLods, triggers its lod meshes to update
    zox_system(
        vox_lods_mesh_trigger_system,
        zoxp_update,
        [none] chunks3.Chunk3,
        [none] chunks3.VoxLods,
        [none] chunks3.ChunkUpdate,
    );
    // NOTE: Writes to VoxelNode
    zox_system(
        clone_vox_system,
        zoxp_octree_write,
        [out] CloneVox,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] blocks.BlockScale,
        [out] chunks3.ChunkSize,
        [out] colorz.ColorRGBs,
        [none] chunks3.Chunk3,
    );
    zox_system(
        combine_vox_system,
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
        chunk_colors_build_system,
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
    // Builds lod meshes from their parent VoxLods
    zox_system(
        vox_lod_mesh_build_system,
        zoxp_octree_read,
        [in] rendering.RenderDepth,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshColorRGBs,
        [none] chunks3.VoxLodMesh,
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
        block_scale_system,
        zoxp_update,
        [in] rendering.RenderDepthDirty,
        [in] rendering.RenderDepth,
        [out] blocks.BlockScale,
        [none] chunks3.Chunk3,
    );
    zox_system(
        chunk_entities_lod_system,
        zoxp_update,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDistance,
        [in] chunks3.ChunkEntities,
        [none] chunks3.Chunk3,
    );
    zox_system(
        chunk_sides_trigger_system,
        zoxp_update,
        [none] chunks3.Chunk3,
        [none] chunks3.ChunkUpdate,
        // [none] chunks.ChunkTextured,
    );
    zox_system(
        chunk_neighbors_sides_trigger_system,
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
        chunk_mesh_spawn_system,
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
        chunk_mesh_spawn2_system,
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
        chunk_mesh_transition_system,
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
        voxel_update_queue_system,
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
