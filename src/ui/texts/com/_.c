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
// zox_tag(TextDirty);
zox_tag(TextWrap);            // lines wrap to its LayoutSize (text_wrap, text_columns), i.e. dialogue: a fixed box, not sized to its text
zoxc_entity(GlyphPrefab);
// Animated text: types TargetText out a character at a time (i.e. dialogue)
zoxc_fixed_string(TargetText, 512);
zoxc_double(AnimateTextBegin);
zoxc_float(AnimateTextTime);
zoxc_float2(AnimateTextTimeLimits);
zoxc_state(GlyphSpawnedDirty);
zoxc_state(AnimateTextEnded);

void zox_components_texts(ecs *world) {
    zoxd_tag(Text);
    zoxd_tag(TextLabel);
    zoxd_tag(TextResizer);
    zoxd_tag(TextRenderEnabler);
    zoxd_tag(TextInput);
    zoxd_byte(TextAlignment);
    zoxd_byte(TextResolution);
    zoxd_byte2(TextPadding);
    zoxd_tag(TextWrap);
    zoxd_int(TextFontSize);
    zoxd_double(AnimateText);
    zoxd_text(TextData);
    // zoxd_tag(TextDirty);
    zoxd_entity(GlyphPrefab);
    zoxd_fixed_string(TargetText);
    zoxd_double(AnimateTextBegin);
    zoxd_float(AnimateTextTime);
    zoxd_float2(AnimateTextTimeLimits);
    zoxd_state(GlyphSpawnedDirty);
    zoxd_state(AnimateTextEnded);
}
