/*
 *  Module: Games
 *
 *      - Games sits above Players
 *      - Handles Game Orchestration
 *      - Spawns Terrain and Player Characters
 *
 * */
// TODO: Remove GameStateTarget and just set directly
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

zox_begin_module(Games) {
    define_components_games(world);
    zox_define_systems_games(world);
    add_hook_terminal_command(process_arguments_games);
    add_hook_spawn_prefabs(spawn_prefabs_games);
    // zox_module_dispose(dispose_games);
    // initialize_events_games();
} zox_end_module(Games);

#endif
