#ifndef zoxm_users_ui
#define zoxm_users_ui

entity prefab_window_users;

entity spawn_prefab_window_users(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("window_users");
    zox_prefab_set(e, FramePrefabLink, { prefab_frame });
    return e;
}

void spawn_prefabs_users_ui(ecs* world) {
    prefab_window_users = spawn_prefab_window_users(world, prefab_window);
}

zox_begin_module(UsersUI)
    add_hook_spawn_prefabs(spawn_prefabs_users_ui);
zox_end_module(Users)

#endif
