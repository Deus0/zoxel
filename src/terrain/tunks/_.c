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

// TODO: Chunk2NeighborsSystem
// TODO: Chunk2LodSystem
// TODO: Height Map Update based on LODs

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
