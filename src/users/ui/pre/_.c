entity spawn_prefab_window_users(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("window_users");
    zox_prefab_set(e, FramePrefabLink, { prefab_frame });
    return e;
}

entity prefab_window_users;

void spawn_prefabs_users_ui(ecs* world) {
    prefab_window_users = spawn_prefab_window_users(world, prefab_window);
}
