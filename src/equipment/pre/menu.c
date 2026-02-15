entity spawn_prefab_menu_equipment(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("menu_equipment");
    zox_add_tag(e, MenuEquipment);
    return e;
}
