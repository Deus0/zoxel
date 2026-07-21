entity spawn_popup3(ecs *world, SpawnDataElement3 data, Text3DData text_data, Zigel3DData zigel_data, SpawnDataPopup3 popup_data) {
#ifdef zox_safety_checks
    if (!text_data.prefab) {
        zox_log_error("invalid text_data prefab in spawn_popup3D");
        return 0;
    } else if (!zox_valid(data.prefab)) {
        zox_log_error("prefab_popup is invalid");
        return 0;
    }
#endif
    float3 depth_position = (float3) { 0, 0, element3D_depth_difference };
    zox_instance(data.prefab);
    zox_set_unique_name(e, "popup3");
    zox_set(e, FillColor, { data.base_color });
    if (popup_data.lifetime) {
        zox_set(e, DestroyInTime, { popup_data.lifetime });
    }
    text_data.position = depth_position;
    text_data.parent = e;
    zigel_data.position = depth_position;
    zigel_data.scale = popup_data.scale;
    entity text = spawn_text3D(world, text_data, zigel_data);
    zox_set_unique_name(text, "popup_text");
    zox_set_parent(world, text, e);
    return e;
}

entity spawn_popup3_easy(ecs *world, const char *text, color text_color, float3 position, float scale, float lifetime) {
    SpawnDataElement3 popup_spawn_data = {
        .prefab = prefab_popup3D,
        .base_color = (color) { 0, 0, 0, 0 }, // background color
    };
    Text3DData text_data = {
        .prefab = prefab_text3D, //popup3D,
        .text = text,
    };
    Zigel3DData zigel_data = {
        .prefab = prefab_zigel3D,
        .resolution = text3D_resolution,
        .font_thickness = text3D_fill_thickness,
        .font_outline = text3D_outline_thickness,
        .fill_color = text_color,
        .outline_color = (color) { 15, 15, 15, 255 }
    };
    SpawnDataPopup3 popup_data = {
        scale = scale,
        lifetime = lifetime,
    };
    entity e = spawn_popup3(world, popup_spawn_data, text_data, zigel_data, popup_data);
    zox_set(e, Position3D, { position });
    if (lifetime) {
        float3 end = float3_add(position, float3_scale(float3_up, randf_range(0.1f, 0.15f)));
        lerp_to_position(world, e, 0.02, randf_range(0.4f, 0.6f), position, end);
    }
    return e;
}