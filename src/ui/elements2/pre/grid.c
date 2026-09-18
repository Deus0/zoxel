entity spawn_prefab_grid(
    ecs *world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("grid");
    // Grid
    zox_setv(e, GridPadding, byte2_zero);
    zox_setv(e, GridMargins, byte2_zero);
    zox_setv(e, GridDirty, zox_dirty_trigger);
    return e;
}
