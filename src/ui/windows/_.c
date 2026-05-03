#ifndef zoxm_windows
#define zoxm_windows

#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(Windows)
    define_components_windows(world);
    define_systems_windows(world);
    add_hook_spawn_prefabs(spawn_prefabs_windows);
zox_end_module(Texts)

#endif
