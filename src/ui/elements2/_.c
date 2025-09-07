#ifndef zox_elements2
#define zox_elements2

#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Elements2)
    define_components_elements2(world);
    define_systems_elements2(world);
    add_hook_spawn_prefabs(spawn_prefabs_elements2);
zox_end_module(Elements2)

#endif