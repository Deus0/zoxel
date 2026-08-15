entity spawn_prefab_menu_stats(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("menu_stats");
    zox_add(e, MenuStats);
    return e;
}
