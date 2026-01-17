zox_tag(FontStyle);
zox_tag(TTFFontStyle);
zox_tag(Font);
zox_tag(FontTexture);
zox_tag(Zigel);
zoxc_byte(FontThickness);
zoxc_byte(FontOutlineThickness);
zoxc_byte(ZigelIndex);
zoxc_color(FontFillColor);
zoxc_color(FontOutlineColor);
zoxc_entity(FontLink);
zoxc_arrayd(FontData, byte2)

void define_components_zigels(ecs *world) {
    zoxd_tag(FontStyle);
    zoxd_tag(TTFFontStyle);
    zoxd_tag(Font);
    zoxd_tag(FontTexture);
    zoxd_tag(Zigel);
    zoxd_byte(FontThickness);
    zoxd_byte(FontOutlineThickness);
    zoxd_byte(ZigelIndex);
    zoxd_color(FontFillColor);
    zoxd_color(FontOutlineColor);
    zoxd_arrayd(FontData);
    zoxd_entity(FontLink);
}