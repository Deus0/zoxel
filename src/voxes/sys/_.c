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
    // NOTE: timing specific, fucks up if changes position
    zox_system(
        Bounds3GrowSystem,
        zoxp_update,
        [in] chunks3.VoxelNodeDirty, // rendering.MeshDirty,
        [in] chunks3.ChunkSize,
        [in] blocks.BlockScale,
        [out] transforms3.Bounds3D,
        [out] transforms3.Bounds3Dirty
    );
    zox_system(
        Bounds3EnableSystem,
        zoxp_update,
        transforms3.Bounds3Dirty,
        [out] physics.DisableMovement,
        [out] physics.DisableGravity
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
        [out] chunks3.VoxelNodeDirty
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
        [out] chunks3.VoxelNodeDirty
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
        [out] chunks.FindNeighbors,
        [out] chunks3.ChunkNeighbors,
        [none] chunks3.ChunkTextured    // we should just check if parent has chunk links here
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
        [out] rendering.TextureDirty,
        [none] textures.VoxTexture
    );
}
