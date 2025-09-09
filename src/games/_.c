#ifndef zoxm_games
#define zoxm_games

#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"
#include "games2/_.c"
#include "games2/_.c"

zox_begin_module(Games)
    initialize_events_games();
    zox_module_dispose(dispose_games);
    add_hook_terminal_command(process_arguments_games);
    add_hook_spawn_prefabs(spawn_prefabs_games);
    define_components_games(world);
    define_systems_games(world);
zox_end_module(Games)

#endif