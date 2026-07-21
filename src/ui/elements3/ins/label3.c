entity spawn_label3D(ecs *world, SpawnDataElement3 data, Text3DData text_data, Zigel3DData zigel_data) {
    if (!text_data.prefab) {
        zox_loge("invalid text_data prefab in spawn_label3D\n")
        return 0;
    }
    zox_instance(data.prefab);
    zox_set_unique_name(e, "label3");
    zox_set(e, RenderDisabled, { data.render_disabled });
    if (!is_color_null(data.base_color)) {
        zox_set(e, FillColor, { data.base_color });
    }
    if (!is_color_null(data.outline_color)) {
        zox_set(e, OutlineColor, { data.outline_color });
    }
    // sub text
    text_data.position = (float3) { 0, 0, element3D_depth_difference };
    zigel_data.position = text_data.position;
    text_data.parent = e;
    entity text = spawn_text3D(world, text_data, zigel_data);
    zox_set_parent(world, text, e);
    if (data.ui_holder) {
        zox_set(e, UIHolderLink, { data.ui_holder });
        zox_set(e, UITrail, { { 0, data.trail_offset, 0 } });
    } else {
        zox_loge("No UIHolder for UI");
    }
    return e;
}
