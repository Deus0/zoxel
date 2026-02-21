#include "flatlands.c"
#include "grasslands.c"
#include "realm.c"
#include "debug_bounds.c"
#include "realm_tilemaps.c"
#include "linking.c"
#include "spawn.c"
#include "death.c"

realm_clear_system(BlockLinks);

zox_declare_system_state_event(RealmBlocks, GenerateRealm, zox_generate_realm_blocks, spawn_realm_blocks)
zox_declare_system_state_event(RealmTilemaps, GenerateRealm, zox_generate_realm_tilemaps, spawn_realm_tilemaps)


// Note: Updates on VoxelNode has to be done in PostLoad, away from use of Voxels, due to the cleaning step
void define_systems_terrain(ecs *world) {

    realm_clear_systemd(blocks, BlockLinks);

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
        [in] voxes.VoxLink,
        [out] chunks.NodeDepth,
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
        [in] voxes.VoxLink,
        [in] tunks.TunkLink,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] chunks3.VoxelNodeDirty,
        [out] chunks3.VoxelNodeGenerated,
        [out] chunks3.VoxelNodeLoaded,
        [none] !FlatlandChunk,
        [none] TerrainChunk
    );

    // Streaming Terrain Chunks
    zox_filter(
        streamers,
        [in] streaming.StreamPoint,
        [none] streaming.Streamer
    );
    zox_system_ctx_1(
        ChunkSpawnSystem,
        zoxp_mainthread,
        streamers,
        [in] chunks3.ChunkPosition,
        [in] voxes.VoxLink,
        [in] rendering.RenderDistance,
        [out] chunks3.ChunkNeighbors,
        [none] streaming.StreamedChunk
    );
    zox_system(
        Chunk3DeathSystem,
        zoxp_destroy,
        [in] voxes.VoxLink,
        [in] chunks3.ChunkPosition,
        [in] rendering.RenderDistance,
        [in] rendering.RenderDepth,
        [none] streaming.StreamedChunk
    );

    // Debug Terrains
#ifdef zox_debug_chunk_bounds
    zox_system_1(
        ChunkBoundsDrawSystem,
        zoxp_mainthread,
        [in] transforms3.Position3D,
        [in] transforms3.Bounds3D,
        [in] rendering.RenderDisabled,
        [none] terrain.TerrainChunk
    );
#endif

}
