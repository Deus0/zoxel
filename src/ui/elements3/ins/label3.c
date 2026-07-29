entity2 spawn_label3D(ecs *world, SpawnDataElement3 data, Text3DData text_data, Zigel3DData zigel_data, float3 trail_offset) {
    if (!text_data.prefab) {
        zox_loge("invalid text_data prefab in spawn_label3D\n")
        return (entity2) { };
    }
    entity canvas = zox_ins(world, prefab_canvas3);
    // Panel / Label
    zox_instance(data.prefab);
    zox_set_unique_name(e, "label3");
    zox_set_parent(world, e, canvas);
    zox_set(e, RenderDisabled, { data.render_disabled });
    zox_set(e, FillColor, { data.base_color });
    zox_set(e, OutlineColor, { data.outline_color });
    // sub text
    text_data.position = (float3) { 0, 0, element3D_depth_difference };
    // zigel_data.position = text_data.position;
    text_data.parent = e;
    entity text = spawn_text3D(world, text_data, zigel_data);
    zox_set_parent(world, text, e);
    if (data.ui_holder) {
        zox_set(canvas, UIHolderLink, { data.ui_holder });
        zox_set(canvas, UITrail, { trail_offset });
    } else {
        zox_loge("No UIHolder for UI");
    }
    return (entity2) { canvas, text };
}
