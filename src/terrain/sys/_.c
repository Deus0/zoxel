#include "build_chunk.c"
#include "render.c"
#include "flatlands.c"
#include "grasslands.c"
#include "realm.c"
#include "debug_bounds.c"
#include "realm_tilemaps.c"
#include "linking.c"

zox_declare_system_state_event(ClearRealmBlocks, GenerateRealm, zox_generate_realm_clear, clear_realm_blocks)
zox_declare_system_state_event(RealmBlocks, GenerateRealm, zox_generate_realm_blocks, spawn_realm_blocks)
zox_declare_system_state_event(RealmTilemaps, GenerateRealm, zox_generate_realm_tilemaps, spawn_realm_tilemaps)

// Note: Updates on VoxelNode has to be done in PostLoad, away from use of Voxels, due to the cleaning step
void define_systems_terrain(ecs *world) {

    zox_define_system_state_event_1(ClearRealmBlocks, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);
    zox_define_system_state_event_1(RealmBlocks, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);
    zox_define_system_state_event_1(RealmTilemaps, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);

    zox_system(
        ChunkLinkSystem,
        EcsOnUpdate,
        [in] terrain.TerrainLink,
        [in] transforms3.Position3D,
        [out] chunks3.ChunkPosition,
        [out] chunks3.ChunkLink,
        [none] chunks3.LinkChunk
    );

    // generate terrain
    zox_system(
        FlatlandSystem,
        zoxp_voxels_write,
        [in] chunks3.GenerateChunk,
        [in] chunks3.ChunkPosition,
        [in] chunks3.VoxLink,
        [out] chunks3.NodeDepth,
        [out] chunks3.VoxelNode,
        [none] TerrainChunk,
        [none] FlatlandChunk
    );
    zox_system(
        GrassyPlainsSystem,
        zoxp_voxels_write,
        [in] chunks3.ChunkPosition,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDepthDirty,
        [in] chunks3.VoxelNodeEdited,
        [in] chunks3.VoxLink,
        [out] chunks3.VoxelNode,
        [out] chunks3.NodeDepth,
        [out] chunks3.VoxelNodeDirty,
        [out] chunks3.VoxelNodeGenerated,
        [out] chunks3.VoxelNodeLoaded,
        [none] !FlatlandChunk,
        [none] TerrainChunk
    );

    if (!headless) {
        // move this into chunk3, for chunk3_textured
        zox_system(
            Chunk3BuildSystem,
            zoxp_voxels_read,
            [in] chunks3.VoxLink,
            [in] chunks3.ChunkMeshDirty,
            [in] chunks3.VoxelNode,
            [in] rendering.RenderDepth,
            [in] chunks3.ChunkNeighbors,
            [in] blocks.BlockScale,
            [out] rendering.MeshIndicies,
            [out] rendering.MeshVertices,
            [out] rendering.MeshUVs,
            [out] rendering.MeshColorRGBs,
            [out] rendering.MeshDirty,
            [none] chunks3.ChunkTextured
        );

        // move this into chunk3, for chunk3_textured
        zox_render3D_system(
            Chunk3RenderSystem,
            [in] transforms3.TransformMatrix,
            [in] rendering.MeshGPULink,
            [in] rendering.UvsGPULink,
            [in] rendering.ColorsGPULink,
            [in] rendering.MeshIndiciesGpu,
            [in] chunks3.VoxLink,
            [in] rendering.RenderDisabled
        );
    }

#ifdef zox_debug_chunk_bounds
    zox_system_1(
        ChunkBoundsDrawSystem,
        zoxp_mainthread,
        [in] transforms3.Position3D,
        [in] generic.Bounds3D,
        [in] rendering.RenderDisabled,
        [none] TerrainChunk
    );
#endif

}
