entity spawn_prefab_town(ecs* world) {
    zox_prefab();
    zox_add_tag(e, Town);
    zox_prefab_set(e, Seed, { 0 });
    zox_prefab_set(e, GenerateName, { zox_dirty_trigger });
    zox_prefab_set(e, ZoxName, { 0 });
    zox_prefab_set(e, Generate, { zox_dirty_trigger });
    zox_prefab_set(e, BlockPosition2, { int2_zero });
    zox_prefab_set(e, TownSize, { byte2_zero });
    zox_prefab_set(e, WallHeight, { 2 });
    zox_prefab_set(e, WallThickness, { 3 });
    return e;
}
