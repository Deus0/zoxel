// todo: find a way to add game modules onto game menu - add stats ui on etc
#ifndef zoxm_game_ui
#define zoxm_game_ui

// todo: use a byte index instead for menu type
#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(GameUI)
    zox_module_dispose(dispose_gameui);
    add_hook_spawn_prefabs(spawn_prefabs_game_ui);
    initialize_hook_taskbar();
    define_components_game_ui(world);
    define_systems_game_ui(world);
    add_taskbar_button((hook_taskbar) {
        .index = 0,
        .spawn = &spawn_menu_paused,
        .component_id = MenuPaused,
        .texture_name = "paused"
    });
zox_end_module(GameUI)

#endif