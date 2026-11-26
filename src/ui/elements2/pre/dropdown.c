static entity prefab_dropdown;

static entity spawn_prefab_dropdown(ecs* world, entity button) {
    zox_prefab_child(button);
    zox_prefab_name("dropdown");
    zox_setv(e, DropdownIndex, 0);
    return e;
}
