entity spawn_prefab_menu_crafting(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("menu_crafting");
    zox_add(e, MenuCrafting);
    return e;
}
