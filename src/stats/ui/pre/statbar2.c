entity spawn_prefab_statbar2(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("statbar2");
    zox_add_tag(e, Statbar);
    zox_prefab_set(e, StatLink, { 0 });
    return e;
}