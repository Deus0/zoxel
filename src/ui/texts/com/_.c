zox_tag(Text);
zox_tag(TextLabel);
zox_tag(TextResizer);
zox_tag(TextRenderEnabler);
zox_tag(TextInput);
zoxc_byte(TextAlignment);
zoxc_byte(TextResolution);
zoxc_byte2(TextPadding);
zoxc_int(TextFontSize);
zoxc_double(AnimateText);
zoxc_text(TextData)
zoxc_state(TextDirty);
zoxc_state(TextSizeDirty);
zoxc_entity(GlyphPrefab);

void define_components_texts(ecs *world) {
    zoxd_tag(Text);
    zoxd_tag(TextLabel);
    zoxd_tag(TextResizer);
    zoxd_tag(TextRenderEnabler);
    zoxd_tag(TextInput);
    zoxd_byte(TextAlignment);
    zoxd_byte(TextResolution);
    zoxd_byte2(TextPadding);
    zoxd_int(TextFontSize);
    zoxd_double(AnimateText);
    zoxd_text(TextData);
    zoxd_state(TextDirty);
    zoxd_state(TextSizeDirty, [none] Text);
    zoxd_entity(GlyphPrefab);
}
