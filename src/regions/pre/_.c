entity spawn_prefab_region(ecs* world) {
    zox_prefab();
    zox_add_tag(e, Region);
    zox_set(e, RegionPosition, { int3_zero });
    return e;
}

entity prefab_region;

void spawn_prefabs_regions(ecs* world) {
    prefab_region = spawn_prefab_region(world);
}
