/*
 * +------------------------------------------------------------------+
 * | Zox Module: Players                                              |
 * |                                                                  |
 * |  Profiles - Progress - Identity - Stats - Player State           |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#include "sta/_.c"
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_players(ecs* world) {
    zox_module(players);
    zox_components_players(world);
    zox_systems_players(world);
    add_hook_on_boot(on_boot_players);
    add_hook_spawn_prefabs(spawn_prefabs_players);
    // add_to_event_game_state((zox_game_event) { &game_state_players });
}
