entity spawn_prefab_window_users(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("window_users");
    zox_prefab_set(e, FramePrefabLink, { prefab_frame });
    return e;
}