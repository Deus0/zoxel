zox_tag(Zext);
zox_tag(ZextLabel);
zox_tag(ZextRenderEnabler);
zox_tag(Text2D);
zoxc_byte(TextDirty);
zoxc_byte(ZextAlignment);
zoxc_byte(TextResolution);
zoxc_byte2(TextPadding);
zoxc_int(TextSize);
zoxc_double(AnimateZext);
zoxc_text(TextData)

void define_components_texts(ecs *world) {
    zoxd_tag(Zext);
    zoxd_tag(ZextLabel);
    zoxd_tag(ZextRenderEnabler);
    zoxd_tag(Text2D);
    zoxd_byte(TextDirty);
    zoxd_byte(ZextAlignment);
    zoxd_byte(TextResolution);
    zoxd_byte2(TextPadding);
    zox_define_component_int(TextSize);
    zox_define_component_double(AnimateZext);
    zoxd_text(TextData);
}