/*entity spawn_prefab_menu_paused(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("menu_paused");
    zox_add_tag(e, MenuPaused);
    zox_set(e, EntityInitialize, { zox_dirty_trigger });
    zox_set(e, TaskbarToggleLink, { 0 });
    return e;
}*/