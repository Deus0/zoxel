entity spawn_prefab_frame_label_action(
    ecs* world
) {
    zox_prefab_child(prefab_zext);
    zox_add_tag(e, ItemIconLabel);
    // zox_add_tag(e, SkillIconLabel);
    return e;
}

entity spawn_prefab_frame_action(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("frame_action");
    zox_prefab_set(e, ActiveState, { 0 });
    zox_prefab_set(e, ActiveStateDirty, { 0 });

    entity prefab_frame_label = spawn_prefab_frame_label_action(world);
    zox_prefab_set(e, LabelPrefabLink, { prefab_frame_label });

    return e;
}