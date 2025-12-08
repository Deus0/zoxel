#ifndef zoxm_controllers2
#define zoxm_controllers2

// todo: movement should be done in one system, and we can use a Move float2 in our character - Movement2 and Movement3 - should be a float -1 to 1, and determine intention to move
#include "dat/settings.c"
zox_tag(Player2D);
zox_tag(PlayerCharacter2D);
zoxc_float2(Movement2);
#include "pre/_.c"
#include "sys/_.c"
#include "fun/label.c"

zox_begin_module(Controllers2)
    zoxd_tag(Player2D);
    zoxd_tag(PlayerCharacter2D);
    zoxd_float2(Movement2);
    define_systems_controllers2(world);
    add_hook_spawn_prefabs(spawn_prefabs_players2D);
zox_end_module(Controllers2)

#endif