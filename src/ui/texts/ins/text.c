entity spawn_text(ecs *world, entity prefab, entity parent, int2 position, float2 position_anchor, byte font_size, byte alignment, byte2 padding, const char* text, color fill, color outline) {
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
    if (length) {
        tdata.length = length;
        zox_set_ptr(e, TextData, tdata);
        zox_set(e, TextDirty, { zox_dirty_trigger });
    }
    return e;
}
