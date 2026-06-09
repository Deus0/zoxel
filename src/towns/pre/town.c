entity spawn_prefab_town(ecs* world) {
    zox_prefab();
    zox_add_tag(e, Town);
    zox_set(e, Generate, { zox_dirty_trigger });
    zox_set(e, VoxelPosition, { int3_zero });
    return e;
}
