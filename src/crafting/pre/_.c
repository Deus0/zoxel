#include "menu.c"

entity prefab_menu_crafting;

void spawn_prefabs_crafting(ecs* world) {
    prefab_menu_crafting = spawn_prefab_menu_crafting(
        world,
        prefab_window);
    zox_setv(prefab_menu_crafting, FramePrefabLink, prefab_frame);
    if (prefab_realm) {
        zox_prefab_add(prefab_realm, RecipeLinks);
    }
}
