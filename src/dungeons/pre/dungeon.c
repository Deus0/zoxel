entity spawn_prefab_dungeon(ecs* world) {
    zox_prefab();
    zox_add_tag(e, Dungeon);
    zox_prefab_set(e, Seed, { 0 });
    zox_prefab_set(e, GenerateName, { zox_dirty_trigger });
    zox_prefab_set(e, Generate, { zox_dirty_trigger });
    zox_prefab_set(e, BlockPosition2, { int2_zero });
    // zox_prefab_set(e, DungeonSize, { byte2_zero });
    return e;
}
