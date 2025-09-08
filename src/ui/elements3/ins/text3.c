float3 calculate_zigel3D_position(
    const float2 zigel3D_size,
    const int data_index,
    const int zigels_count,
    float scale
) {
    if (scale == 0) {
        scale = 1;
    }
    float3 position = float3_zero;
    position.x += zigel3D_size.x * 0.5f * scale;
    position.x -= zigel3D_size.x * 0.5f * zigels_count * scale; // centre
    position.x += zigel3D_size.x * data_index * scale;
    return position;
}

entity spawn_text3D(
    ecs *world,
    const Text3DData data,
    Zigel3DData zigel_data
) {
    zox_instance(data.prefab);
    zox_name("text3D");
    zox_set(e, ParentLink, { data.parent });
    zox_set(e, LocalPosition3D, { data.position });
    zox_set(e, TextPadding, { data.padding });
    zox_set(e, MeshAlignment, { data.alignment });
    zox_set(e, FontThickness, { zigel_data.font_thickness });
    zox_set(e, FontFillColor, { zigel_data.fill_color });
    zox_set(e, FontOutlineColor, { zigel_data.outline_color });
    zox_set(e, Text3DScale, { zigel_data.scale });
    zox_set(e, TextFontSize, { zigel_data.resolution });
    // text
    const int length = data.text != NULL ? strlen(data.text) : 0;
    TextData text = (TextData) { 0, NULL };
    initialize_TextData(&text, length);
    for (int i = 0; i < text.length; i++) {
        text.value[i] = convert_ascii(data.text[i]);
    }
    zox_set_ptr(e, TextData, text);
    zox_set(e, TextDirty, { zox_dirty_trigger });

    return e;
}