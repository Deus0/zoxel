entity spawn_prefab_menu_stats(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("menu_stats");
    zox_add_tag(e, MenuStats);
    return e;
}
