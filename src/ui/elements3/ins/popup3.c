entity spawn_popup3(
    ecs *world,
    const SpawnDataElement3D data,
    Text3DData text_data,
    Zigel3DData zigel_data,
    const SpawnDataPopup3 popup_data
) {
    if (!text_data.prefab) {
        zox_log_error("invalid text_data prefab in spawn_popup3D");
        return 0;
    } else if (!zox_valid(data.prefab)) {
        zox_log_error("prefab_popup is invalid");
        return 0;
    }
    float3 depth_position = (float3) { 0, 0, element3D_depth_difference };
    zox_instance(data.prefab);
    zox_name("popup3D");
    zox_set(e, Color, { data.base_color });
    if (popup_data.lifetime) {
        zox_set(e, DestroyInTime, { popup_data.lifetime });
    }
    // Children children = (Children) { 0 };
    text_data.position = depth_position;
    text_data.parent = e;
    zigel_data.position = depth_position;
    zigel_data.scale = popup_data.scale; // 2;
    entity text = spawn_text3D(world, text_data, zigel_data);
    // zox_set(text, Scale1D, { 4 })
    zox_set_unique_name(text, "popup_text");
    // add_to_Children(&children, text);
    zox_set_parent(world, text, e);
    // zox_set_ptr(e, Children, children);
    // zox_log_error("spawned popup [%lu]", e);
    return e;
}