/*
 * Bodys are Combining Voxes
 *
 *      - Items
 *      - Body UI
 *
 *      Used By:
 *          - Characters
 *
*/
#ifndef zoxm_bodys
#define zoxm_bodys

// #include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Bodys)
    define_components_bodys(world);
    define_systems_bodys(world);
    add_hook_spawn_prefabs(spawn_prefabs_bodys);
    // initialize_settings_models(world);
    add_taskbar_button((hook_taskbar) {
        .index = 6,
        .spawn = &spawn_player_menu_body,
        .component_id = MenuBody,
        .texture_name = "taskbar_body",
        .tooltip_text = "Body"
    });
zox_end_module(Bodys)

#endif
