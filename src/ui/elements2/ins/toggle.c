entity spawn_toggle(ecs *world, LayoutParentData canvas, LayoutParentData parent, ElementSpawnData element, SpawnTextData text, SpawnButtonData button, byte value) {
    entity e = spawn_button_old(world, canvas, parent, element, text, button);
    // TODO: Toggle Prefab
    zox_add(e, Toggle);
    zox_set(e, ActiveState, { value });
    zox_set(e, ActiveStateDirty, { zox_dirty_trigger });
    zox_set(e, ActiveColor, { button_outline_active });
    return e;
}
