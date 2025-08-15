ecs_entity_t spawn_prefab_main_start(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab)
    zox_prefab_name("menu_start")
    zox_add_tag(e, MenuStart)
    return e;
}