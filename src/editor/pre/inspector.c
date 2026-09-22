entity spawn_prefab_inspector(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("inspector");
    zox_add(e, EditorElement);
    zox_setv(e, ElementFontSize, 16);
    zox_setv(e, InspectorDirty, zox_dirty_trigger);
    return e;
}
