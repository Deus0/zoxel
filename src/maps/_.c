/*
 * +--------------------------------------------------+
 * | Zox Module: Maps                                 |
 * |                                                  |
 * |  Regions - World Map - Markers - Navigation      |
 * |                                                  |
 * +--------------------------------------------------+
 */
// TODO: Fix updating of maps
// - black textures appear
// - need to not regenerate them all, reuse them

byte zox_maps_flip_x = 0;
byte zox_maps_flip_z = 1;
#define zox_dbg_maps_regions 1
#define zox_dbg_maps_heights 2
#define zox_dbg_maps_vegetation 3
#define zox_dbg_maps_towns 4
#define zox_dbg_maps_end 5
byte zox_dbg_maps = 0;

void zox_dbg_map_cycle() {
    zox_dbg_maps++;
    if (zox_dbg_maps == zox_dbg_maps_end) {
        zox_dbg_maps = 0;
    }
    zox_log("[zox_dbg_maps] Set to [%i]", zox_dbg_maps);
}

// TODO: Just Destroy/Spawn map pieces instead
// TODO: we can even downsize the pieces?
int minimap_size = 80;
byte bigmap_zoom = 16;
float bigmap_alpha = 0.8f;
byte minimap_zoom = 4;
float minimap_alpha = 0.46f;
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

void import_maps(ecs* world) {
    zox_module(maps);
    define_components_maps(world);
    zox_define_systems_maps(world);
    add_hook_spawn_prefabs(spawn_prefabs_maps);
    add_hook_spawn_prefabs(zox_events_maps);
    add_taskbar_button((hook_taskbar) {
        .index = 8,
        .spawn = &spawn_player_menu_map,
        .component_id = MenuMap,
        .texture_name = "taskbar_map",
        .tooltip_text = "Map"
    });
}
