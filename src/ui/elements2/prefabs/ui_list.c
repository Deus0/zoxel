entity spawn_prefab_ui_list(ecs *world, entity prefab) {

    zox_prefab_child(prefab)
    zox_prefab_name("ui_list")

    zox_prefab_set(e, ListUIMax, { 0 });
    zox_prefab_set(e, ListPadding, { { 8, 8 } });
    zox_prefab_set(e, ListMargins, { { 8, 8 } });
    zox_prefab_set(e, ListStart, { 0 });
    zox_prefab_set(e, ListVisible, { 0 });
    zox_prefab_set(e, ListDirty, { zox_dirty_trigger });

    return e;
}