entity spawn_toggle(
    ecs *world,
    LayoutParentData canvas,
    LayoutParentData parent,
    ElementSpawnData element,
    SpawnTextData text,
    SpawnButtonData button,
    byte value
) {
    entity e = spawn_button(
        world,
        canvas,
        parent,
        element,
        text,
        button
    );

    // zox_remove_tag(e, Button);
    // TODO: Toggle Prefab
    zox_add_tag(e, Toggle);
    zox_set(e, ToggleState, { value });
    zox_set(e, ToggleStateDirty, { zox_dirty_trigger });

    return e;
}