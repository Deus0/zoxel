// #include "flatlands.c"
#include "realm.c"
#include "game_start.c"
#include "debug_bounds.c"
#include "linking.c"
// Lights (Move to lights)
// generation
#include "render_depth.c"
// Streaming
// #include "stream_death.c"
#include "stream.c"
#include "stream_unload.c"
#include "water_flow.c"
#include "water_view.c"

// Note: Updates on VoxelNode has to be done in PostLoad, away from use of Voxels, due to the cleaning step
void zox_systems_terrain(ecs *world) {
    // flowing water (water_flow.c): main thread, it writes other chunks' queues and its terrain's flows
    //  - in zoxp_render with the other main thread systems: in zoxp_update, after multi threaded ones, it added two
    //    merges (tst pipeline_syncs); still before zoxp_pre_end clears VoxelNodePostDirty, its edits apply next frame
    zox_system_1(
        water_flow_edits_system,
        zoxp_render,
        [in] chunks3.ChunkPosition,
        [in] chunks.NodeDepth,
        [in] chunks3.VoxelNodeQueue,
        [none] chunks3.VoxelNodePostDirty,
        [none] terrains.TerrainChunk,
    );
    zox_system_1(
        water_flow_system,
        zoxp_render,
        [out] terrains.WaterFlows,
        [none] terrains.Terrain,
    );
    // multi threaded: its tag changes are deferred, sound_underwater is set by the first main camera's alone
    zox_system(
        camera_underwater_system,
        zoxp_update,
        [in] transforms3.Position3D,
        [none] cameras.ViewProjectionMatrix,
        [none] !cameras.CameraUI,
    );
    zox_system(
        chunk_link_system,
        zoxp_update,
        [in] transforms3.Position3D,
        [out] chunks3.ChunkPosition,
        [none] chunks3.LinkChunk
    );
    // Starts Building
    zox_system(
        chunk_lod_system,
        zoxp_update,
        [in] rendering.RenderDepth,
        [out] chunks3.ChunkLodDirty,
        [out] chunks.NodeDepth,
        [none] terrains.TerrainChunk,
        [none] !core.Initialize,
    );
    // NOTE: Dies when out of range
    // Streaming Terrain Chunks
    zox_filter(
        streamers,
        [in] streaming.StreamDirty,
        [in] streaming.StreamerLevel,
        [in] streaming.StreamPosition,
        [out] streaming.StreamRow,
        [out] streaming.StreamSwept,
        [none] streaming.Streamer
    );
    zox_system_ctx(
        terrain_stream_system,
        zoxp_update,
        streamers,
        [in] tunks.TunkLinks,
        [out] streaming.TerrainSpawnQueue,
        [none] terrains.Terrain
    );
    zox_system_1(
        terrain_stream_queue_system,
        zoxp_spawn,
        [in] core.Seed,
        [in] blocks.BlockScale,
        [in] chunks.NodeDepth,
        [out] streaming.TerrainSpawnQueue,
        [out] regions.RegionLinks,
        [out] tunks.TunkLinks,
        [out] chunks3.ChunkLinks,
        [none] terrains.Terrain,
    );
    // left land, a little a frame (sys/stream_unload.c): main thread, it edits its terrain's links
    zox_system_1(
        tunk_unload_system,
        zoxp_spawn,
        [in] tunks.TunkPosition,
        [in] tunks.Chunk3Stack,
        [none] terrains.TunkUnloading,
    );
    // Debug Terrains
#ifdef zox_debug_chunk_bounds
    zox_system_1(
        chunk_bounds_draw_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] transforms3.Bounds3D,
        [in] rendering.RenderDisabled,
        [none] terrains.TerrainChunk
    );
#endif
}

// the game's state changed (its core Dirty): terrain spawns
void game_state_terrain_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GameState, state);
        game_state_terrain(world, e, state->value);
    }
}

void zox_events_terrain(ecs *world) {
    zox_on_add(game_state_terrain_event, [in] games.GameState, [none] games.Game, [none] core.Dirty);
}

