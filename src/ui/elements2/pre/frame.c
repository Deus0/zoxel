// a small square frame ui with an icon in it
// used for game - action/skill/stat - uis
entity spawn_prefab_frame(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("frame");
    zox_add_tag(e, FixToLayout);
    zox_add_tag(e, Frame);
    // zox_prefab_set(e, FillColor, { fill });
    // zox_prefab_set(e, OutlineColor, { outline });
    return e;
}

entity spawn_prefab_frame_toggleable(ecs *world, entity prefab, color outline, color active) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_name("frame_toggleable");
    zox_prefab_set(e, OutlineColor, { outline });
    zox_prefab_set(e, ElementOutlineColor, { outline });
    zox_prefab_set(e, ActiveState, { 0 });
    zox_prefab_set(e, ActiveStateDirty, { 0 });
    zox_prefab_set(e, ActiveColor, { active });
    return e;
}
