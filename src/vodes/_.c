/*
 *  Module Vodes
 *
 *      - Basically World Blocks
 *      - Chests, Doors, Grass, etc
 *      - Block Health too
 *      - Anything that uses entities on top of chunks for Voxels
 *
 * */
#ifndef zoxm_vodes
#define zoxm_vodes

// TODO: Use better states for spawning / updating Vodes

#include "vodes3/_.c"

zox_begin_module(Vodes) {
    zox_import_module(Vodes3);
} zox_end_module(Vodes);

#endif
