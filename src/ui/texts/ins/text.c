entity spawn_text(ecs *world, SpawnZext data) {
    if (!zox_valid(data.canvas.e)) {
        zox_logw("Canvas invalid in [spawn_zext]");
        return 0;
    }
    if (!zox_valid(data.element.prefab)) {
        zox_logw("Invalid Prefab in [spawn_zext]");
        return 0;
    }
    int2 texture_size;
    byte font_resolution;
    if (data.zext.font_resolution) {
        font_resolution = data.zext.font_resolution;
        texture_size = int2_single(data.zext.font_resolution);
    } else {
        font_resolution = data.zext.font_size;
        texture_size = int2_single(data.zext.font_size);
    }
    int zext_data_length = data.zext.text != NULL ? strlen(data.zext.text) : 0;
    TextData text_data = (TextData) { 0 };
    initialize_TextData(&text_data, zext_data_length);
    for (int i = 0; i < text_data.length; i++) {
        text_data.value[i] = convert_ascii(data.zext.text[i]);
    }
    int2 pixel_size = calculate_zext_size(text_data.value, text_data.length, data.zext.font_size, data.zext.margins, default_line_padding);
    zox_instance(data.element.prefab);
    zox_name("text");
    if (!zox_valid(e)) {
        zox_loge("Invalid e in [spawn_zext]");
        return 0;
    }
    initialize_element(world, e, data.parent.e, data.canvas.e, data.element.position, pixel_size, texture_size, data.element.anchor, data.element.layer);
    zox_set(e, RenderDisabled, { data.element.render_disabled });
    zox_set(e, TextFontSize, { data.zext.font_size });
    zox_set(e, TextResolution, { font_resolution });
    zox_set(e, TextPadding, { data.zext.margins });
    zox_set(e, TextAlignment, { data.zext.alignment });
    zox_set(e, MeshAlignment, { data.zext.alignment });
    zox_set(e, FontFillColor, { data.zext.font_fill_color });
    zox_set(e, FontOutlineColor, { data.zext.font_outline_color });
    if (data.zext.font_thickness) {
        zox_set(e, FontThickness, { data.zext.font_thickness });
    }
    if (data.zext.font_outline_thickness) {
        zox_set(e, FontOutlineThickness, { data.zext.font_outline_thickness });
    }
    zox_set_ptr(e, TextData, text_data);
    zox_set(e, TextDirty, { zox_dirty_trigger });
    return e;
}


entity spawn_text_new(ecs *world, entity prefab, entity parent, int2 position, float2 position_anchor, byte font_size, byte alignment, byte2 padding, const char* text, color fill, color outline) {
    byte font_resolution = font_size;
    if (font_resolution < 8) {
        font_resolution = 8;
    }
    zox_instance(prefab);
    zox_name("text");
    zox_set_parent(world, e, parent);
    zox_set(e, Anchor, { position_anchor });
    zox_set(e, LayoutPosition, { position });
    zox_set(e, TextFontSize, { font_size });
    zox_set(e, TextPadding, { padding });
    zox_set(e, TextAlignment, { alignment });
    zox_set(e, MeshAlignment, { alignment });
    zox_set(e, TextResolution, { font_resolution });
    zox_set(e, FontFillColor, { fill });
    zox_set(e, FontOutlineColor, { outline });
    // Text
    TextData tdata = (TextData) { 0 };
    uint length;
    tdata.value = convert_text_data(text, &length);
    tdata.length = length;
    zox_set_ptr(e, TextData, tdata);
    if (length > 0) {
        zox_set(e, TextDirty, { zox_dirty_trigger });
    }
    /*int2 size = calculate_zext_size(tdata.value, length, font_size, padding, default_line_padding);
    zox_set(e, LayoutSize, { size });
    zox_set(e, TextureSize, { size });*/
    return e;
}
