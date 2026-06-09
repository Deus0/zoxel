entity spawn_prefab_region(ecs* world) {
    zox_prefab();
    zox_add_tag(e, Region);
    zox_set(e, RegionPosition, { int2_zero });
    zox_set(e, Generate, { zox_dirty_trigger });
    zox_set(e, VoxelPosition, { int3_zero });
    zox_set(e, VoxelSize, { int3_zero });
    return e;
}
