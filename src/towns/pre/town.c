entity spawn_prefab_town(ecs* world) {
    zox_prefab();
    zox_add(e, Town);
    zox_set(e, Seed, { 0 });
    zox_set(e, GenerateName, { 1 });
    zox_set(e, GenerateTown, { 1 });
    zox_set(e, ZoxName, { 0 });
    zox_set(e, Generate, { zox_dirty_trigger });
    zox_set(e, BlockPosition2, { int2_zero });
    zox_set(e, TownSize, { byte2_zero });
    zox_set(e, WallHeight, { 2 });
    zox_set(e, WallThickness, { 3 });
    return e;
}
