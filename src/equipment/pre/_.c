#include "menu.c"

entity prefab_menu_equipment;

void spawn_prefabs_equipment(ecs* world) {

    prefab_menu_equipment = spawn_prefab_menu_equipment(world, prefab_window_users);
    // zox_set(prefab_menu_equipment, FramePrefabLink, { prefab_frame_item });

    // prefab_item = spawn_prefab_item(world);
    if (prefab_character3) {
        zox_prefab_character_add(EquipLinks);
    }
    if (prefab_realm) {
        zox_prefab_add(prefab_realm, EquipLinks);
    }
}