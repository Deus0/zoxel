entity spawn_prefab_hierarchy(ecs *world, entity p) {

    zox_prefab_child(p);
    zox_prefab_name("hierarchy");

    zox_add_tag(e, EditorElement);
    zox_prefab_set(e, WindowType, { zox_window_hierarchy });
    zox_prefab_set(e, ElementFontSize, { 16 });

    zox_prefab_set(e, HierarchyUIDirty, { zox_dirty_trigger });

    return e;
}
