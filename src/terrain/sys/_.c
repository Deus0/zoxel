// #include "flatlands.c"
#include "realm.c"
#include "game_start.c"
#include "debug_bounds.c"
#include "linking.c"
// Lights (Move to lights)
#include "generated.c"
// Streaming
#include "stream_first.c"
#include "stream_grow.c"
#include "stream_lod.c"
#include "stream_death.c"
// generation
#include "render_depth.c"
#include "landfill.c"
#include "vegetation.c"

// Note: Updates on VoxelNode has to be done in PostLoad, away from use of Voxels, due to the cleaning step
void define_systems_terrain(ecs *world) {
    zox_system(
        ChunkLinkSystem,
        EcsOnUpdate,
        [in] terrains.TerrainLink,
        [in] transforms3.Position3D,
        [out] chunks3.ChunkPosition,
        [out] chunks3.ChunkLink,
        [out] physics.DisableMovement,
        [none] chunks3.LinkChunk
    );
    zox_system(
        Chunk3DeathSystem,
        zoxp_destroy,
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
        [none] terrains.TerrainChunk
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
    // Starts Building
    zox_system(
        RenderDepthChunk3System,
        zoxp_voxels_write,
        [in] saves.Loaded,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDepthDirty,
        [out] chunks.NodeDepth,
        [out] core.Generate,
        [out] core.Busy,
        [none] terrains.TerrainChunk
    );
    zox_system(
        LandfillChunk3System,
        zoxp_voxels_write,
        [in] core.Generate,
        [in] tunks.TunkLink,
        [in] chunks3.ChunkPosition,
        [in] chunks.NodeDepth,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [none] terrains.TerrainChunk
    );
    zox_system(
        VegetationChunk3System,
        zoxp_voxels_write,
        [in] core.Generate,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkPosition,
        [in] tunks.TunkLink,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [none] terrains.TerrainChunk
    );
    // Lighting
    zox_system(
        Chunk3GeneratedSystem,
        EcsOnUpdate,
        [in] core.Generate,
        [out] lights.GenerateLights,
        [none] terrains.TerrainChunk,
        [none] lights3.SunnyChunk
    );
    // Streaming Terrain Chunks
    zox_system_1(
        FirstTerrainChunkSystem,
        zoxp_mainthread,
        [in] streaming.StreamerLevel,
        [in] streaming.StreamLink,
        [in] streaming.StreamPoint,
        [in] streaming.StreamDirty,
        [none] streaming.Streamer
    );
    zox_filter(
        streamers,
        [in] streaming.StreamerLevel,
        [in] streaming.StreamLink,
        [in] streaming.StreamPoint,
        [none] streaming.Streamer
    );
    zox_system_ctx_1(
        ChunkSpawnSystem,
        zoxp_mainthread,
        streamers,
        [in] chunks3.ChunkPosition,
        [in] rendering.RenderDistance,
        [out] chunks3.ChunkNeighbors,
        [none] streaming.StreamedChunk
    );
    zox_filter(
        streamers_lod,
        [in] streaming.StreamDirty,
        [in] streaming.StreamerLevel,
        [in] streaming.StreamLink,
        [in] streaming.StreamPoint,
        [none] streaming.Streamer
    );
    zox_system_ctx(
        ChunkLodSystem,
        zoxp_update,
        streamers_lod,
        [in] chunks3.ChunkPosition,
        [out] rendering.RenderDepth,
        [out] rendering.RenderDistance,
        [out] rendering.RenderDepthDirty,
        [out] rendering.RenderDistanceDirty,
        [out] core.Busy,
        [none] streaming.StreamedChunk
    );
}
