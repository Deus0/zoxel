/*
 *  Chunks3 Colored
 *
 *      - Specifically for our game models
 *      - Npcs
 *      - Grass
 *      - Clouds
 *      - Even Textured Blocks, they get baked down
 *
 * */
#ifndef zoxm_chunks3_colored
#define zoxm_chunks3_colored

// TODO: Use Sides Data for building
// TODO: Proper AO using all cube neighbors
byte zox_split_colored_chunks = 0;
byte zox_ambient_occlusion = 1;
#define zox_ambient_occlusion27
#include "dat/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Chunks3Colored) {
    define_systems_chunks3_colored(world);
} zox_end_module(Chunks3Textured);

#endif
