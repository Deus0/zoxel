entity spawn_prefab_menu_skills(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("menu_skills");
    zox_add_tag(e, MenuSkills);
    return e;
}