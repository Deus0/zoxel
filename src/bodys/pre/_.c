#include "menu.c"

entity prefab_menu_body;

void spawn_prefabs_bodys(ecs* world) {

    prefab_menu_body = spawn_prefab_menu_body(world, prefab_window_users);

    if (prefab_character3) {
        zox_prefab_character_add(BodyLinks);
        zox_prefab_character_add(BodyDirty);
    }

    if (prefab_realm) {
        zox_prefab_add(prefab_realm, BodyLinks);
    }
}
