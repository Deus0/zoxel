#if !defined(zoxm_game)
#define zoxm_game

#include "fun/_.c"
#include "set/_.c"
#include "sys/_.c"

zox_begin_module(ZoxGame)

    game_name = "zoxel";

    initialize_zoxel_settings(world);

    define_systems_zoxel(world);
    add_hook_spawn_prefabs(zoxel_on_spawn_prefabs);

    add_hook_on_boot(spawn_weather);

    // our test window
    add_hook_key_down(spawn_test_list);

zox_end_module(ZoxGame)

#endif
