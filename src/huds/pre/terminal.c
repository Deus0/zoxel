entity spawn_prefab_menu_terminal(ecs *world, const entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("menu_terminal");
    zox_add_tag(e, MenuTerminal);
    return e;
}
