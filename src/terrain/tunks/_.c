/**
 *  Tunks - Chunk2s
 *
 *      - Heightmaps
 *      - Biomes
 *      - Precipation
 *      - Temperature
 *
 * */
#ifndef zoxm_tunks2
#define zoxm_tunks2

// TODO: Biome Map
// TODO: Vegetation Map for Grass  / trees etc
// TODO: Maps Update based on LODs??
// TODO: Quadtrees for map data?

// TODO: Use sbyte instead of byte?
// TODO: Chunk2NeighborsSystem

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Tunks)
    define_components_tunks(world);
    define_systems_tunks(world);
    add_hook_spawn_prefabs(spawn_prefabs_tunks);
zox_end_module(Tunks)

#endif
