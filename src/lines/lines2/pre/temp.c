entity spawn_prefab_temp(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_set(e, DestroyInTime, { 0 });
    return e;
}
