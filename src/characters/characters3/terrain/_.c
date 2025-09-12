#ifndef zoxm_characters3_terrain
#define zoxm_characters3_terrain

#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "dbg/_.c"
#include "sys/_.c"

zox_begin_module(Characters3Terrain)
    add_hook_spawn_prefabs(spawn_prefabs_characters3_terrain);
    define_components_characters3_terrain(world);
    define_systems_characters3_terrain(world);
zox_end_module(Characters3Terrain)

#endif
