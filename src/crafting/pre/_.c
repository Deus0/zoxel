#include "menu.c"

entity prefab_menu_crafting;

void spawn_prefabs_crafting(ecs* world) {
    prefab_menu_crafting = spawn_prefab_menu_crafting(world, prefab_window_users);
    /*if (prefab_character3) {
        zox_prefab_character_add(CraftLinks);
    }*/
    if (prefab_realm) {
        zox_prefab_add(prefab_realm, RecipeLinks);
    }
}
