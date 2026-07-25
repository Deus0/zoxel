float3 calculate_zigel3D_position(float2 zigel3D_size, int data_index, int zigels_count, float scale) {
    if (scale == 0) {
        scale = 1;
    }
    float3 position = float3_zero;
    position.x = zigel3D_size.x * (data_index - (zigels_count - 1) * 0.5f) * scale;
    position.z = element3D_depth_difference;
    return position;
}

entity spawn_text3D(ecs *world, Text3DData data, Zigel3DData zigel_data) {
    zox_instance(data.prefab);
    zox_set_unique_name(e, "text3");
    zox_set_parent(world, e, data.parent);
    zox_set(e, LocalPosition3D, { data.position });
    zox_set(e, TextPadding, { data.padding });
    zox_set(e, MeshAlignment, { data.alignment });
    zox_set(e, FontThickness, { zigel_data.font_thickness });
    zox_set(e, FontOutlineThickness, { zigel_data.font_outline });
    zox_set(e, FontFillColor, { zigel_data.fill_color });
    zox_set(e, FontOutlineColor, { zigel_data.outline_color });
    zox_set(e, Text3DScale, { zigel_data.scale });
    zox_set(e, TextFontSize, { zigel_data.resolution });
    // text
    TextData text_data = (TextData) { 0 };
    uint length;
    text_data.value = convert_text_data(data.text, &length);
    if (length) {
        text_data.length = length;
        zox_set_ptr(e, TextData, text_data);
        zox_set(e, TextDirty, { zox_dirty_trigger });
        // zox_log("Text3D [%s]", data.text);
    }
    return e;
}
