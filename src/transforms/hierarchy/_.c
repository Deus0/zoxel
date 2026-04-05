#ifndef zoxm_hierarchy
#define zoxm_hierarchy

#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Hierarchys)
    define_components_hierarchy(world);
    define_systems_hierarchy(world);
zox_end_module(Hierarchys)

#endif
