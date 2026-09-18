entity spawn_prefab_list(
    ecs* world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("ui_list");
    // List
    zox_set(e, ListPadding, { { 8, 8 } });
    zox_set(e, ListMargins, { { 8, 8 } });
    zox_setv(e, ListStart, 0);
    zox_setv(e, ListVisible, 0);
    zox_setv(e, ListAlignment, zox_alignment_centre);
    zox_setv(e, ListDirty, zox_dirty_trigger);
    zox_setv(e, ListPositionDirty, zox_dirty_trigger);
    return e;
}
