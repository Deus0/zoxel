/**
 *  Zox Terrain
 *
 *      - colored vox models
 *
 *      - todo: don't generate mesh until render is enabled!
 *      - todo: use ChunkOriginDistance instead of RenderDepth for camera distance for TerrainChunks
  *      - todo: delay BlockVoxSpawnSystem to main thread pipeline like character spawning, use same trigger? rest on load? make a resetsystem 2 for this - just  make reset defines use the pipeline
 *
 * */
#if !defined(zoxm_terrain) && defined(zoxm_chunks3)
#define zoxm_terrain

// Chunk States
#define zox_generate_tchunk_start 1
#define zox_generate_tchunk_landfill 1
#define zox_generate_tchunk_vegetation 2
#define zox_generate_tchunk_towns 3
#define zox_generate_tchunk_end 4

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
