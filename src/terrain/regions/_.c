/**
 *  Regions - (MegaChunks)
 *
 *  * Each NxM Chunk3s will have a region *
 *
 *      - Town Points
 *      - Highways
 *      - Dungeon Points
 *
 * */
#ifndef zoxm_regions
#define zoxm_regions

// TODO: Spawn and link Regions
// TODO: Town Spawn Point

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Regions)
    define_components_regions(world);
    define_systems_regions(world);
    add_hook_spawn_prefabs(spawn_prefabs_regions);
zox_end_module(Regions)

#endif
