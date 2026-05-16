// entity prefab_menu_actions;
entity prefab_frame_action;
entity prefab_icon_action;
#include "frame.c"
#include "icon.c"
zox_user_menu_functions_minimal(Actions, Action, actions, action)

void spawn_prefabs_ui_actions(ecs *world) {
    prefab_frame_action     = spawn_prefab_frame_action(world, prefab_frame_user);
    prefab_icon_action      = spawn_prefab_icon_action(world, prefab_icon_user);
    // prefab_menu_actions     = spawn_prefab_menu_actions(world, prefab_window_users);
    // zox_set(prefab_menu_actions, FramePrefabLink, { prefab_frame_action });
}
