#include "menu.c"

entity prefab_menu_map;

void spawn_prefabs_maps(ecs* world) {
    prefab_menu_map = spawn_prefab_menu_map(world, prefab_window);
}