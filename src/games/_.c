/*
 * +------------------------------------------------------------------+
 * | Zox Module: Games                                                   |
 * |                                                                  |
 * |  Sessions - Modes - Rules - Worlds - Game State                    |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 *      - Games sits above Players
 *      - Handles Game Orchestration
 *      - Spawns Terrain and Player Characters
 *
 *  TODO:
 *
 *      - Remove GameStateTarget and just set directly
 *
 * */
#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"
#include "games2/_.c"
#include "games2/_.c"

void import_games(ecs* world) {
    zox_module(games);
    zox_components_games(world);
    zox_systems_games(world);
    add_hook_terminal_command(process_arguments_games);
    add_hook_spawn_prefabs(spawn_prefabs_games);
    // zox_module_dispose(dispose_games);
    // initialize_events_games();
}

