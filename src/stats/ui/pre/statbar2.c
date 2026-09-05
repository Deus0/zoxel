entity spawn_prefab_statbar2(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("statbar2");
    zox_add(e, Statbar);
    return e;
}
