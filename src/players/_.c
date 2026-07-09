#if !defined(zoxm_players)
#define zoxm_players

// todo: move based functionality into here
#include "sta/_.c"
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(Players) {
    zox_define_components_players(world);
    zox_define_systems_players(world);
    add_hook_on_boot(on_boot_players);
    add_hook_spawn_prefabs(spawn_prefabs_players);
    // add_to_event_game_state((zox_game_event) { &game_state_players });
} zox_end_module(Players);

#endif
