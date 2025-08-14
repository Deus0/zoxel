#ifndef zoxm_lighting3
#define zoxm_lighting3

#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "dbg/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Lighting3)
    define_components_lighting3(world);
    define_systems_lighting3(world);
zox_end_module(Lighting3)

#endif