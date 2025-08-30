entity spawn_prefab_frame_user(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("frame_user");
    zox_prefab_set(e, UserDataLink, { 0 });
    zox_prefab_add(e, TooltipEvent);
    return e;
}