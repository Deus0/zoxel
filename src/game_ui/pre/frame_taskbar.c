entity spawn_prefab_frame_taskbar(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_set(e, ActiveState, { 0 });
    zox_prefab_set(e, ActiveStateDirty, { 0 });
    return e;
}
