entity spawn_prefab_home(ecs* world) {
    zox_prefab();
    zox_add(e, Home);
    zox_set(e, Seed, { 0 });
    // zox_set(e, Generate, { zox_dirty_trigger });
    zox_set(e, BlockPosition2, { int2_zero });
    zox_set(e, HomeSize, { byte2_zero });
    // zox_set(e, WallHeight, { 2 });
    // zox_set(e, WallThickness, { 3 });
    return e;
}
