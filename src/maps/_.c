#ifndef zoxm_maps
#define zoxm_maps

#define zox_dbg_maps_regions 1
#define zox_dbg_maps_heights 2
#define zox_dbg_maps_vegetation 3
#define zox_dbg_maps_towns 4
byte zox_dbg_maps = 0;
// TODO: Just Destroy/Spawn map pieces instead
// TODO: we can even downsize the pieces?
byte bigmap_zoom = 16;
float bigmap_alpha = 0.8f;
byte minimap_zoom = 4;
float minimap_alpha = 0.36f;
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Maps) {
    define_components_maps(world);
    zox_define_systems_maps(world);
    add_hook_spawn_prefabs(spawn_prefabs_maps);
    add_taskbar_button((hook_taskbar) {
        .index = 8,
        .spawn = &spawn_player_menu_map,
        .component_id = MenuMap,
        .texture_name = "taskbar_map",
        .tooltip_text = "Map"
    });
} zox_end_module(Maps);

#endif
