entity spawn_prefab_ui_list(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("ui_list");
    // List
    zox_prefab_set(e, ListPadding, { { 8, 8 } });
    zox_prefab_set(e, ListMargins, { { 8, 8 } });
    zox_prefab_set(e, ListStart, { 0 });
    zox_prefab_set(e, ListVisible, { 0 });
    zox_prefab_set(e, ListAlignment, { zox_alignment_centre });
    zox_prefab_set(e, ListDirty, { zox_dirty_trigger });
    zox_prefab_set(e, ListPositionDirty, { zox_dirty_trigger });
    // Texture
    /*zox_add_tag(e, FixToLayout);
    // Selection
    zox_add_tag(e, Selectable);
    zox_prefab_set(e, SelectState, { 0 });
    // Clicking
    zox_add_tag(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });*/
    return e;
}
