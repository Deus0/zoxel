entity spawn_prefab_inspector(ecs *world, entity p) {

    zox_prefab_child(p);
    zox_prefab_name("inspector");

    zox_add_tag(e, EditorElement);
    zox_prefab_set(e, WindowType, { zox_window_inspector });
    zox_prefab_set(e, ElementFontSize, { 16 });

    zox_prefab_set(e, InspectorDirty, { zox_dirty_trigger });

    return e;
}
