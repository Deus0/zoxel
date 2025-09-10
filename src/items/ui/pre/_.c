ecs_entity_t prefab_menu_items;
ecs_entity_t prefab_icon_item;
ecs_entity_t prefab_frame_item;
#include "icon.c"
#include "frame.c"
#include "menu.c"

void spawn_prefabs_ui_items(ecs_world_t *world) {
    prefab_icon_item = spawn_prefab_icon_item(world, prefab_icon_user);
    prefab_frame_item = spawn_prefab_frame_item(world, prefab_frame);
    prefab_menu_items = spawn_prefab_menu_items(world, prefab_window_users);
    zox_set(prefab_menu_items, FramePrefabLink, { prefab_frame_item });
}