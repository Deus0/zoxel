#ifndef zoxm_dungeons
#define zoxm_dungeons

#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Dungeons)
    add_hook_spawn_blocks(&spawn_block_dungeon_blocks);
    add_hook_spawned_block(&spawn_world_dungeon_core);
    define_components_dungeons(world);
    define_systems_dungeons(world);
zox_end_module(Dungeons)

#endif