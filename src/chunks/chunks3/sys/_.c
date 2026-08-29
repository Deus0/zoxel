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

void define_systems_chunks3(ecs *world) {
    // ColoredChunk, triggers mesh to update
    zox_system(
        ColoredChunkMeshTriggerSystem,
        zoxp_update,
        [none] chunks.Chunk,
        [none] chunks3.ColorChunk,
        [none] chunks3.VoxelNodePostDirty,
    );
    zox_system(
        VoxelUpdateQueueSystem,
        zoxp_queue_pre_clear,
        [in] chunks.NodeDepth,
        [out] chunks3.VoxelNodeQueue,
        [out] chunks3.VoxelNode,
        [none] chunks.Chunk,
        [none] chunks3.Chunk3,
    );
    // NOTE: Removes Dirty at end of frame
    zox_system(
        VoxelOctreeOptimizeSystem,
        zoxp_remove,
        [out] chunks3.VoxelNode,
        [none] chunks.Chunk,
        [none] chunks3.Chunk3,
        [none] chunks3.VoxelNodeDirty,
    );
    zox_system(
        VoxelNodePostDirtySystem,
        zoxp_remove,
        [none] chunks.Chunk,
        [none] chunks3.Chunk3,
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
    zox_system(
        BakeVoxSystem,
        zoxp_update,
        [in] rendering.ModelLink,
        [in] textures.TextureLinks
    );
    // Move to Voxes Module
    zox_system(
        ChunkFindNeighborSystem,
        zoxp_update,
        [in] chunks3.ChunkPosition,
        [out] chunks3.ChunkNeighbors,
        [none] chunks.FindNeighbors,
        [none] chunks.ChunkTextured,    // we should just check if parent has chunk links here
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
        VoxTextureSystem,
        zoxp_update,
        [in] rendering.TextureSize,
        [in] blocks.VoxBakeSide,
        [in] rendering.ModelLink,
        [out] textures.GenerateTexture,
        [out] textures.TextureData,
        [none] textures.VoxTexture
    );
}
