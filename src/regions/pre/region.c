entity spawn_prefab_region(ecs* world) {
    zox_prefab();
    zox_add_tag(e, Region);
    zox_set(e, RegionPosition, { int2_zero });
    zox_set(e, Generate, { zox_dirty_trigger });
    zox_set(e, BlockPosition2, { int2_zero });
    zox_set(e, BlockSize2, { int2_zero });
    return e;
}
