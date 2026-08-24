// a small square frame ui with an icon in it
// used for game - action/skill/stat - uis
entity spawn_prefab_frame(
    ecs *world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("frame");
    zox_add(e, FixToLayout);
    zox_add(e, Frame);
    return e;
}

entity spawn_prefab_frame_toggleable(
    ecs *world,
    entity prefab,
    color outline,
    color active)
{
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_name("frame_toggleable");
    zox_setv(e, OutlineColor, outline);
    zox_setv(e, ElementOutlineColor, outline);
    zox_setv(e, ActiveState, 0);
    zox_setv(e, ActiveStateDirty, 0);
    zox_setv(e, ActiveColor, active);
    return e;
}
