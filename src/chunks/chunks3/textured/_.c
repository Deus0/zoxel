#ifndef zoxm_chunks3_textured
#define zoxm_chunks3_textured

// TODO: Move terrain cache into functions
// TODO: Cache all managers found, not just single
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
