entity spawn_prefab_grid(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("grid");
    zox_prefab_set(e, GridDirty, { zox_dirty_trigger });
    zox_prefab_set(e, GridPadding, { byte2_zero });
    zox_prefab_set(e, GridMargins, { byte2_zero });
    return e;
}