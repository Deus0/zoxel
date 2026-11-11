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
// stream queue updates per second (was 1 per frame, so streaming slowed with the frame rate)
float stream_updates_per_second = 60;
byte stream_updates_max_per_frame = 8;  // so a long frame doesn't burst
double stream_update_max_wait = 2.0;    // seconds, an update queued longer goes before closer ones
float stream_update_credit = 0;
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
#include "tst/_.c"

void import_terrains(ecs* world) {
    zox_module(terrains);
    zox_components_terrain(world);
    zox_systems_terrain(world);
    zox_tsts_terrain(world);
    add_hook_terminal_command(process_arguments_terrain);
    add_hook_spawn_prefabs(spawn_prefabs_terrain);
    add_hook_spawn_prefabs(zox_events_terrain);
    zox_add_module(terrain_collisions);
}
