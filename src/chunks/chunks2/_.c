#ifndef zoxm_chunks2
#define zoxm_chunks2

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"

zox_begin_module(Chunks2)
    define_components_chunks2(world);
    add_hook_spawn_prefabs(spawn_prefabs_chunks2);
zox_end_module(Chunks2)

#endif
