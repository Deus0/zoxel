// TODO: Move this under terrain!
#ifndef zoxm_terrain_npcs
#define zoxm_terrain_npcs

#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "dbg/_.c"
#include "sys/_.c"

zox_begin_module(TerrainNpcs)
    define_components_characters3_terrain(world);
    define_systems_characters3_terrain(world);
    add_hook_spawn_prefabs(spawn_prefabs_characters3_terrain);
zox_end_module(TerrainNpcs)

#endif
