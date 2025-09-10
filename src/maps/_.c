#ifndef zoxm_maps
#define zoxm_maps

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"

zox_begin_module(Maps)
    define_components_maps(world);
    add_hook_spawn_prefabs(spawn_prefabs_maps);
    add_taskbar_button((hook_taskbar) {
        .index = 7,
        .spawn = &spawn_player_menu_map,
        .component_id = MenuMap,
        .texture_name = "taskbar_map"
    });
zox_end_module(Maps)

#endif