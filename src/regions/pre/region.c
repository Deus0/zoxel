entity spawn_prefab_region(ecs* world) {
    zox_prefab();
    zox_add_tag(e, Region);
    zox_set(e, VoxLink, { 0 });
    zox_set(e, RegionPosition, { int2_zero });
    return e;
}
