entity spawn_prefab_frame_label_stat(ecs* world) {
    zox_prefab_child(prefab_zext);
    zox_add_tag(e, StatIconLabel);
    return e;
}

entity spawn_prefab_frame_stat(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("frame_stat");
    entity prefab_frame_label_stat = spawn_prefab_frame_label_stat(world);
    zox_prefab_set(e, LabelPrefabLink, { prefab_frame_label_stat });
    return e;
}
