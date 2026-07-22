/*
 *  Zoxel Huds
 *
 *      - Game UIs, anything game specific
 *
 *      - Taskbar and handles other game uis
 *
 * */
#ifndef zoxm_huds
#define zoxm_huds

byte zox_disable_screen_fader = 0;

// TODO: spawn unique canvas per viewport, viewports per player
// TODO: use a byte index instead for menu type
// TODO: find a way to add game modules onto game menu - add stats ui on etc

#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"
// #define zox_log_camera_spawning


zox_begin_module(Huds) {
    zox_module_dispose(dispose_gameui);
    add_hook_spawn_prefabs(spawn_prefabs_game_ui);
    initialize_hook_taskbar();
    define_components_game_ui(world);
    define_systems_game_ui(world);
    add_taskbar_button((hook_taskbar) {
        .index = 0,
        .spawn = &spawn_menu_paused,
        .component_id = MenuPaused,
        .texture_name = "taskbar_paused",
        .tooltip_text = "Pause Menu"
    });
    add_hook_on_boot(on_boot_game_ui);
} zox_end_module(Huds);

#endif
