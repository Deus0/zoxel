// #include "flatlands.c"
#include "realm.c"
#include "debug_bounds.c"
#include "linking.c"
#include "spawn.c"
#include "death.c"
#include "game_start.c"
#include "kickstart.c"
#include "generated.c"
// generation
#include "render_depth.c"
#include "landfill.c"
#include "vegetation.c"

// Note: Updates on VoxelNode has to be done in PostLoad, away from use of Voxels, due to the cleaning step
void define_systems_terrain(ecs *world) {
    zox_system(
        ChunkLinkSystem,
        EcsOnUpdate,
        [in] terrain.TerrainLink,
        [in] transforms3.Position3D,
        [out] chunks3.ChunkPosition,
        [out] chunks3.ChunkLink,
        [none] chunks3.LinkChunk
    );
    // Streaming Terrain Chunks
    zox_filter(
        streamers,
        [in] streaming.StreamPoint,
        [none] streaming.Streamer
    );
    zox_system_1(
        FirstTerrainChunkSystem,
        zoxp_mainthread,
        [in] streaming.StreamLink,
        [in] streaming.StreamPoint,
        [in] streaming.StreamDirty,
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
    zox_system_1(
        TerrainGameStartSystem,
        zoxp_mainthread,
        [in] realms.RealmLink,
        [in] games.GameState,
        [in] games.GameStateDirty,
        [none] games.Game
    );
    zox_system(
        RenderDepthChunk3System,
        zoxp_voxels_write,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDepthDirty,
        [in] chunks3.VoxelNodeLoaded,
        [out] chunks.NodeDepth,
        [out] core.Generate,
        [none] TerrainChunk
    );
    zox_system(
        Chunk3GeneratedSystem,
        zoxp_voxels_write,
        [in] core.Generate,
        [in] chunks3.VoxelNodeGenerated,
        [none] terrain.TerrainChunk
    );
    zox_system(
        LandfillChunk3System,
        zoxp_voxels_write,
        [in] core.Generate,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkPosition,
        [in] voxes.VoxLink,
        [in] tunks.TunkLink,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [none] terrain.TerrainChunk
    );
    zox_system(
        VegetationChunk3System,
        zoxp_voxels_write,
        [in] core.Generate,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkPosition,
        [in] voxes.VoxLink,
        [in] tunks.TunkLink,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [none] terrain.TerrainChunk
    );
}
    // generate terrain
    /*zox_system(
        FlatlandSystem,
        zoxp_voxels_write,
        [in] core.Generate,
        [in] chunks3.ChunkPosition,
        [in] voxes.VoxLink,
        [out] chunks.NodeDepth,
        [out] chunks3.VoxelNode,
        [none] TerrainChunk,
        [none] FlatlandChunk
    );*/
