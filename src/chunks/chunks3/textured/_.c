#ifndef zoxm_chunks3_textured
#define zoxm_chunks3_textured

// TODO: Move terrain cache into functions
// TODO: Cache all managers found, not just single
// TODO: Build up adjacent faces in another system

// TODO: Now the Seems are only from chunks that did not generate properly, they are one depth too less in resolution - only in dig mode tho weird - zox_dbg_boost_generation_depth fixed it for now - but shouldn't need to do

#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Chunks3Textured)
    define_systems_chunks3_textured(world);
zox_end_module(Chunks3Textured)

#endif
