entity spawn_prefab_frame_label_action(ecs* world) {
    zox_prefab_child(prefab_zext);
    zox_add_tag(e, ItemIconLabel);
    // zox_add_tag(e, SkillIconLabel);
    return e;
}

entity spawn_prefab_frame_action(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("frame_action");

    prefab_add_active_state(world, e, default_outline_color_frame, color_purple);

    entity prefab_frame_label = spawn_prefab_frame_label_action(world);
    zox_prefab_set(e, LabelPrefabLink, { prefab_frame_label });

    return e;
}