/*
 * +------------------------------------------------------------------+
 * | Zox Module: Terrain                                              |
 * |                                                                  |
 * |  Generation - Chunks - Biomes - Heightmaps - Terrain State       |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 *      - colored vox models
 *      - todo: don't generate mesh until render is enabled!
 *      - todo: use ChunkOriginDistance instead of RenderDepth for camera distance for TerrainChunks
  *      - todo: delay BlockVoxSpawnSystem to main thread pipeline like character spawning, use same trigger? rest on load? make a resetsystem 2 for this - just  make reset defines use the pipeline
 *
 * */
#if !defined(zoxm_terrain) && defined(zoxm_chunks3)
#define zoxm_terrain

byte zox_terrain_chunk_lod_system = 0;
// Initial core
#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
// Core
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "dbg/_.c"
#include "sys/_.c"
#include "collisions/_.c"

zox_begin_module(Terrains) {
    define_components_terrain(world);
    define_systems_terrain(world);
    add_hook_terminal_command(process_arguments_terrain);
    add_hook_spawn_prefabs(spawn_prefabs_terrain);
    zox_import_module(TerrainsCollisions);
} zox_end_module(Terrains);

#endif
