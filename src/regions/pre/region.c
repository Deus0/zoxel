entity spawn_prefab_region(ecs* world) {
    zox_prefab();
    zox_add(e, Region);
    zox_set(e, RegionPosition, { int2_zero });
    zox_set(e, GenerateRegion, { zox_generate_region_start });
    zox_set(e, RegionPosition, { int2_zero });
    zox_set(e, RegionSize, { int2_zero });
    zox_set(e, BlockPosition2, { int2_zero });
    zox_set(e, BlockSize2, { int2_zero });
    return e;
}
