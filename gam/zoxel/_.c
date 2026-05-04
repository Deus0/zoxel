#if !defined(zoxm_game) // && defined(zoxm_players)
#define zoxm_game

#include "set/_.c"
#include "sys/_.c"

zox_begin_module(ZoxGame)

    game_name = "zoxel";

    initialize_zoxel_settings(world);

    define_systems_zoxel(world);
    add_hook_spawn_prefabs(zoxel_on_spawn_prefabs);

    add_hook_on_boot(spawn_weather);

zox_end_module(ZoxGame)

#endif
