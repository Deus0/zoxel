/*
 *  Blocks Module
 *
 *      - Data for the Cubes
 *      - Used by Chunks
 *      - Some Textured, Some Baked
 *
 * */
#ifndef zoxm_blocks
#define zoxm_blocks

float block_place_range = 2;

#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Blocks)
    define_components_blocks(world);
    define_systems_blocks(world);
    add_hook_spawn_prefabs(spawn_prefabs_blocks);
zox_end_module(Blocks)

#endif
