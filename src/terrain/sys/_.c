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
#include "stream2.c"
#include "spawn_queue.c"
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
        [none] chunks3.LinkChunk
    );
    // Starts Building
    zox_system(
        ChunkLodSystem,
        zoxp_update,
        [in] saves.Loaded,
        [in] rendering.RenderDepth,
        [out] chunks3.ChunkLodDirty,
        [out] chunks.NodeDepth,
        [none] terrains.TerrainChunk
    );
    // Lighting
    zox_system(
        SunnyChunkGeneratedSystem,
        zoxp_update,
        [out] chunks.GenerateChunk,
        [none] terrains.TerrainChunk,
        [none] lights3.SunnyChunk,
        [none] !lights.GenerateSunlight
    );
    zox_system(
        ChunkGeneratedSystem,
        zoxp_update,
        [out] chunks.GenerateChunk,
        [none] terrains.TerrainChunk,
        [none] !lights3.SunnyChunk
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
        zox_system_ctx(
            TerrainStreamSystem,
            zoxp_update,
            streamers,
            [in] tunks.TunkLinks,
            [out] streaming.TerrainSpawnQueue,
            [none] terrains.Terrain
        );
        zox_system_1(
            TerrainQueueSystem,
            zoxp_mainthread,
            [in] core.Seed,
            [in] blocks.BlockScale,
            [in] chunks.NodeDepth,
            [out] streaming.TerrainSpawnQueue,
            [out] regions.RegionLinks,
            [out] tunks.TunkLinks,
            [out] voxes.ChunkLinks,
            [none] terrains.Terrain,
        );
        /*zox_system_ctx_1(
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
        );*/
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
}
