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
#include "stream.c"
// generation
#include "render_depth.c"
#include "landfill.c"
#include "vegetation.c"
byte dbg_use_new_streaming = 1;

// Note: Updates on VoxelNode has to be done in PostLoad, away from use of Voxels, due to the cleaning step
void define_systems_terrain(ecs *world) {
    zox_system(
        ChunkLinkSystem,
        zoxp_update,
        [in] terrains.TerrainLink,
        [in] transforms3.Position3D,
        [out] chunks3.ChunkPosition,
        [out] chunks3.ChunkLink,
        [out] physics.DisableMovement,
        [none] chunks3.LinkChunk
    );
    // Starts Building
    zox_system(
        RenderDepthChunk3System,
        zoxp_update,
        [in] saves.Loaded,
        [in] rendering.RenderDepth,
        [out] chunks3.ChunkLodDirty,
        [out] chunks.NodeDepth,
        [out] chunks.GenerateChunk,
        [none] terrains.TerrainChunk
    );
    zox_system(
        LandfillChunk3System,
        zoxp_update,
        [in] tunks.TunkLink,
        [in] chunks3.ChunkPosition,
        [in] chunks.NodeDepth,
        [out] chunks.GenerateChunk,
        [out] chunks3.VoxelNode,
        [none] terrains.TerrainChunk
    );
    zox_system(
        VegetationChunk3System,
        zoxp_update,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkPosition,
        [in] tunks.TunkLink,
        [out] chunks.GenerateChunk,
        [out] chunks3.VoxelNode,
        [none] terrains.TerrainChunk
    );
    // Lighting
    zox_system(
        SunnyChunkGeneratedSystem,
        zoxp_update, // EcsPreUpdate,
        [in] chunks.GenerateChunk,
        [out] lights.GenerateLights,
        [none] terrains.TerrainChunk,
        [none] lights3.SunnyChunk
    );
    zox_system(
        ChunkGeneratedSystem,
        zoxp_update,
        [out] chunks.GenerateChunk,
        [out] chunks3.VoxelNodeDirty,
        [none] terrains.TerrainChunk
    );
    // NOTE: Dies when out of range
    // Streaming Terrain Chunks
    if (dbg_use_new_streaming) {
        zox_filter(
            streamers,
            [in] streaming.StreamDirty,
            [in] streaming.StreamerLevel,
            [in] streaming.StreamLink,
            [in] streaming.StreamPosition,
            [none] streaming.Streamer
        );
        zox_system_ctx_1(
            TerrainStreamSystem,
            zoxp_mainthread,
            streamers,
            [in] core.Seed,
            [in] blocks.BlockScale,
            [in] chunks.NodeDepth,
            [out] regions.RegionLinks,
            [out] tunks.TunkLinks,
            [out] voxes.ChunkLinks,
            [none] terrains.Terrain
        );
    } else {
        zox_system_1(
            FirstTerrainChunkSystem,
            zoxp_mainthread,
            [in] streaming.StreamerLevel,
            [in] streaming.StreamLink,
            [in] streaming.StreamPosition,
            [in] streaming.StreamDirty,
            [none] streaming.Streamer
        );
        zox_filter(
            streamers_grow,
            [in] streaming.StreamerLevel,
            [in] streaming.StreamLink,
            [in] streaming.StreamPosition,
            [none] streaming.Streamer
        );
        zox_system_ctx_1(
            ChunkSpawnSystem,
            zoxp_mainthread,
            streamers_grow,
            [in] rendering.RenderDistanceDirty,
            [in] rendering.RenderDistance,
            [in] chunks3.ChunkPosition,
            [out] chunks3.ChunkNeighbors,
            [none] streaming.StreamedChunk
        );
        zox_system(
            Chunk3DeathSystem,
            zoxp_destroy,
            [in] rendering.RenderDistanceDirty,
            [in] rendering.RenderDistance,
            [in] chunks3.ChunkPosition,
            [none] streaming.StreamedChunk
        );
    }
    if (zox_terrain_chunk_lod_system) {
        zox_filter(
            streamers_lod,
            [in] streaming.StreamDirty,
            [in] streaming.StreamerLevel,
            [in] streaming.StreamLink,
            [in] streaming.StreamPosition,
            [none] streaming.Streamer
        );
        zox_system_ctx(
            TerrainChunkLodSystem,
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
    zox_system_1(
        TerrainGameStartSystem,
        zoxp_mainthread,
        [in] realms.RealmLink,
        [in] games.GameState,
        [in] games.GameStateDirty,
        [none] games.Game
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
    add_system_process_counter(world, zox_id(LandfillChunk3System));
    add_system_process_counter(world, zox_id(VegetationChunk3System));
}
