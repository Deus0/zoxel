entity spawn_toggle(
    ecs *world,
    LayoutParentData canvas,
    LayoutParentData parent,
    ElementSpawnData element,
    SpawnTextData text,
    SpawnButtonData button,
    byte value)
{
    entity e = spawn_button_old(
        world,
        canvas,
        parent,
        element,
        text,
        button);
    // TODO: Toggle Prefab
    zox_add(e, Toggle);
    zox_setv(e, ActiveState, value);
    zox_setv(e, ActiveStateDirty, zox_dirty_trigger);
    zox_setv(e, ActiveColor, button_outline_active);
    return e;
}
