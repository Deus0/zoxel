zox_tag(Zext);
zox_tag(ZextLabel);
zox_tag(ZextRenderEnabler);
zox_tag(Text2D);
zoxc_byte(TextAlignment);
zoxc_byte(TextResolution);
zoxc_byte2(TextPadding);
zoxc_int(TextFontSize);
zoxc_double(AnimateZext);
zoxc_text(TextData)
zoxc_state(TextDirty);
zoxc_state(TextSizeDirty);

void define_components_texts(ecs *world) {
    zoxd_tag(Zext);
    zoxd_tag(ZextLabel);
    zoxd_tag(ZextRenderEnabler);
    zoxd_tag(Text2D);
    zoxd_byte(TextAlignment);
    zoxd_byte(TextResolution);
    zoxd_byte2(TextPadding);
    zox_define_component_int(TextFontSize);
    zoxd_double(AnimateZext);
    zoxd_text(TextData);
    zoxd_state(TextDirty);
    zoxd_state(TextSizeDirty, [none] Zext);
}