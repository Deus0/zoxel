#include "grow_bounds3.c"
#include "vox_texture.c"
#include "clone.c"
#include "combine.c"
#include "bake_vox.c"
#include "find_neighbor.c"
#include "block_scale.c"
#include "entities_lod.c"
#include "move_bounds.c"

void define_systems_voxes(ecs *world) {
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
        [in] voxes.CombineVox,
        [in] voxes.CombineList,
        [in] voxes.CombinePositions,
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
