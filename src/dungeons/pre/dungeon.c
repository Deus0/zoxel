entity spawn_prefab_dungeon(ecs* world) {
    zox_prefab();
    zox_add(e, Dungeon);
    zox_set(e, Seed, { 0 });
    zox_set(e, GenerateName, { 1 });
    zox_set(e, Generate, { zox_dirty_trigger });
    zox_set(e, BlockPosition2, { int2_zero });
    // zox_set(e, DungeonSize, { byte2_zero });
    return e;
}
