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

// TODO: Refactor Grasslands into different systems
//      - LandFillSystem
//      - LandPlantSystem
//      - TownBuildSystem

// Initial core
#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"

// NOTE: these need to be imported before core
#include "voxels/_.c"
#include "block_voxes/_.c"

// Core
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "dbg/_.c"
#include "sys/_.c"

#include "collisions/_.c"

zox_begin_module(Terrain)
    define_components_terrain(world);
    // NOTE: these need to be imported before core
    define_systems_terrain(world);
    initialize_hook_spawn_blocks();
    set_terrain_render_distance();  // update this
    zox_module_dispose(module_dispose_terrain);
    add_hook_terminal_command(process_arguments_terrain);
    // add_to_event_game_state((zox_game_event) { &game_state_terrain });
    add_hook_spawn_prefabs(spawn_prefabs_terrain);
    zox_import_module(TerrainCollisions);
zox_end_module(Terrain)

#endif
