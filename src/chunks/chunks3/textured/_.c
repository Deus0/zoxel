/*
 * Chunk3 Textured!
*/
#ifndef zoxm_chunks3_textured
#define zoxm_chunks3_textured

byte zox_split_textured_quads = 1;
byte zox_dbg_render_all_sides = 0;

// TODO: f2 + f3 = see sometimes  random quads at bottom of chunks, weird asf - now its just occasional mostly fixed
// TODO: Check per split quad if voxel exists
// TODO: Fetch all Block Managers found, not just single
// TODO: Build up adjacent faces in another system

#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Chunks3Textured)
    define_systems_chunks3_textured(world);
zox_end_module(Chunks3Textured)

#endif
