#include "menu.c"

entity prefab_menu_equipment;

void spawn_prefabs_equipment(ecs* world) {
    prefab_menu_equipment = spawn_prefab_menu_equipment(world, prefab_window_users);
}
