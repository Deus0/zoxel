#ifndef zoxm_controllers3
#define zoxm_controllers3

#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"

// TODO: Move chunk spawn code into stream system and out of Controllers


zox_begin_module(Controllers3)
    define_components_controllers3D(world);
    define_systems_controllers3D(world);
    add_hook_spawn_prefabs(spawn_prefabs_controllers3D);
zox_end_module(Controllers3)

#endif