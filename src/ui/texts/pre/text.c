entity spawn_prefab_text(
    ecs* world,
    entity prefab,
    entity prefab_glyph)
{
    zox_prefab_child(prefab);
    zox_prefab_name("text");
    // text
    zox_add(e, Text);
    zox_setv(e, GlyphPrefab, prefab_glyph);
    zox_setv(e, TextData, 0);
    // used for font
    zox_setv(e, TextAlignment, 0);
    zox_setv(e, TextPadding, 0);
    zox_setv(e, TextFontSize, 0);
    zox_setv(e, TextResolution, 0);
    zox_set(e, FontOutlineColor, { { 255, 0, 0, 255 } });
    zox_set(e, FontFillColor, { { 255, 0, 0, 255 } });
    // Do we need to set them? Are font thickness used?
    zox_setv(e, FontThickness, 1);
    zox_setv(e, OutlineThickness, 0); // 1);
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, TextSizeDirty, 0);
    return e;
}
