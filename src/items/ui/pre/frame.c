entity spawn_prefab_frame_label_item(ecs* world) {
    zox_prefab_child(prefab_zext);
    zox_add_tag(e, ItemIconLabel);
    return e;
}

entity spawn_prefab_frame_item(ecs*world, entity prefab) {
    zox_prefab_child(prefab)
    zox_prefab_name("frame_item");
    entity prefab_frame_label = spawn_prefab_frame_label_item(world);
    zox_prefab_set(e, LabelPrefabLink, { prefab_frame_label });
    return e;
}
