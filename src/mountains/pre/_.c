entity spawn_prefab_mountain(ecs* world) {
    zox_prefab();
    zox_add_tag(e, Mountain);
    zox_set(e, Generate, { zox_dirty_trigger });
    zox_set(e, BlockPosition2, { int2_zero });
    zox_set(e, Radius, { 0 });
    zox_set(e, Height, { 0 });
    zox_set(e, Seed, { 0 });
    return e;
}

entity prefab_mountain;

void zox_define_prefabs_mountains(ecs* world) {
    prefab_mountain = spawn_prefab_mountain(world);
}
