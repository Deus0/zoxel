#ifndef zoxm_chunks3_textured
#define zoxm_chunks3_textured

// TODO: Move terrain cache into functions
// TODO: Cache all managers found, not just single
// TODO: Build up adjacent faces in another system

// NOTE: Turns out the dig method was fastest
// We have 3 modes, old, new, and hybrid
byte zox_chunk3_texture_builder_new = 0;
byte zox_chunk3_texture_builder_old = 0;
byte zox_chunk3_texture_builder_mix = 1;

#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Chunks3Textured)
    define_systems_chunks3_textured(world);
zox_end_module(Chunks3Textured)

#endif
