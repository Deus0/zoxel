entity spawn_prefab_menu_equipment(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("menu_equipment");
    zox_add_tag(e, MenuEquipment);
    return e;
}
