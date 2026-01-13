/**
 *  Tunks - Chunk2s
 *
 *      - Heightmaps
 *      - Biomes
 *      - Precipation
 *      - Temperature
 *
 * */
#ifndef zoxm_tunks
#define zoxm_tunks

#include "pre/_.c"
#include "ins/_.c"
// #include "sys/_.c"

zox_begin_module(Tunks)
    add_hook_spawn_prefabs(spawn_prefabs_tunks);
zox_end_module(Tunks)

#endif
