entity prefab_frame_user;
entity prefab_icon_user;
entity prefab_window_users;
#include "frame.c"
#include "icon.c"
#include "window.c"

void spawn_prefabs_users_ui(ecs* world) {
    prefab_window_users = spawn_prefab_window_users(world, prefab_window_invisible);
    if (!prefab_icon) {
        zox_log_error("ui prefab did not spawn");
    } else {
        prefab_icon_user = spawn_prefab_icon_user(world, prefab_icon);
        prefab_frame_user = spawn_prefab_frame_user(world, prefab_frame);
    }
    if (prefab_icon_mouse_follow) {
        zox_prefab_set(prefab_icon_mouse_follow, UserDataLink, { 0 });
    }
}
