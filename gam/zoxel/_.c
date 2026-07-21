#if !defined(zoxm_game)
#define zoxm_game

byte mrpenguin_mode = 1;

// TODO: Generate Block + Model + Colors in Biomes themselves (single point of edit)

#include "fun/_.c"
#include "set/_.c"
#include "sys/_.c"

zox_begin_module(ZoxGame) {
    game_name = "zoxel";
    define_systems_zoxel(world);
    initialize_zoxel_settings(world);
    add_hook_on_boot(spawn_weather);
    // Debug UIs
    add_hook_key_down(zox_dbg_ui_gizmos);
    add_hook_key_down(zox_dbg_ui_cheats);
    add_hook_key_down(zox_dbg_ui_overlays);
    add_hook_key_down(zox_dbg_ui_auto_tests);
    add_hook_key_down(zox_dbg_ui_manual_tests);
} zox_end_module(ZoxGame);

#endif
