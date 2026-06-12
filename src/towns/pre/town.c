entity spawn_prefab_town(ecs* world) {
    zox_prefab();
    zox_add_tag(e, Town);
    zox_set(e, Generate, { zox_dirty_trigger });
    zox_set(e, BlockPosition2, { int2_zero });
    zox_set(e, TownSize, { byte2_zero });
    return e;
}
