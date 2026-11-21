/*
 * +------------------------------------------------------+
 * | Zox Module: Terrain                                  |
 * |                                                      |
 * |  Generation - Chunks - Biomes - Heightmaps - State   |
 * |                                                      |
 * +------------------------------------------------------+
 *
 *      - colored vox models
 *      - todo: don't generate mesh until render is enabled!
 *      - todo: use ChunkOriginDistance instead of RenderDepth for camera distance for TerrainChunks
  *      - todo: delay BlockVoxSpawnSystem to main thread pipeline like character spawning, use same trigger? rest on load? make a resetsystem 2 for this - just  make reset defines use the pipeline
 *
 * */
byte zox_terrain_chunk_lod_system = 0;
// the stream queue's budget a frame (lod changes and spawns, distance only updates are outside it), doc/done/streaming.md
float stream_update_ms = 2;
byte stream_updates_max_per_frame = 8;
byte zox_terrain_no_chunks = 0;   // tunks stream without chunks: their maps only (towns/dbg/maps_dump.c)
float stream_update_aging = 1;  // tunks closer an update counts for each second it waited, so far ones aren't starved
// slots the queue spent, by kind (tests log them: perf_fly), doc/done/streaming.md
int stream_slots_lod = 0, stream_slots_distance = 0, stream_slots_spawn = 0;
// Initial core
#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
// Core
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "eve/_.c"
#include "dbg/_.c"
#include "sys/_.c"
#include "collisions/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

#ifdef zox_tests
// the test level (_/tst zox_tst_add_level): flat land, one chunk far
void zox_tst_level_terrain() {
    zox_flatlands = 1;
    terrain_lod_far = 1;
    render_distance_y = 1;
    zox_terrain_size_set_by_args();
}
#endif

void import_terrains(ecs* world) {
    zox_module(terrains);
    zox_components_terrain(world);
    zox_systems_terrain(world);
#ifdef zox_tests
    zox_tsts_terrain(world);
#endif
    add_hook_terminal_command(process_arguments_terrain);
    add_hook_spawn_prefabs(spawn_prefabs_terrain);
    add_hook_spawn_prefabs(zox_events_terrain);
    zox_add_module(terrain_collisions);
#ifdef zox_tests
    zox_tst_add_level(zox_tst_level_terrain);
#endif
}
