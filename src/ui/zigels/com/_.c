zox_tag(FontStyle);
zox_tag(TTFFontStyle);
zox_tag(Font);
zox_tag(FontTexture);
zox_tag(Zigel);
zox_tag(CentredZigel);
zoxc_byte(FontThickness);
zoxc_byte(FontOutlineThickness);
zoxc_byte(ZigelIndex);
zoxc_ushort(DataIndex);
zoxc_color(FontFillColor);
zoxc_color(FontOutlineColor);
zoxc_entity(FontLink);
zoxc_arrayd(FontData, byte2);
// States
zoxc_byte(ZigelDirty);

#define zox_zigel_dirty_position 1
#define zox_zigel_dirty_update 2
#define zox_zigel_dirty_end 0

void zox_define_components_zigels(ecs *world) {
    zoxd_tag(FontStyle);
    zoxd_tag(TTFFontStyle);
    zoxd_tag(Font);
    zoxd_tag(FontTexture);
    zoxd_tag(Zigel);
    zoxd_tag(CentredZigel);
    zoxd_byte(FontThickness);
    zoxd_byte(FontOutlineThickness);
    zoxd_byte(ZigelIndex);
    zoxd_ushort(DataIndex);
    zoxd_color(FontFillColor);
    zoxd_color(FontOutlineColor);
    zoxd_arrayd(FontData);
    zoxd_entity(FontLink);
    // States
    zoxd_byte(ZigelDirty);
}
