entity spawn_prefab_text(ecs* world, entity prefab, entity prefab_glyph) {
    zox_prefab_child(prefab);
    zox_prefab_name("text");
    // text
    zox_add_tag(e, Zext);
    zox_prefab_set(e, TextData, { 0 });
    zox_prefab_set(e, TextDirty, { 0 });
    zox_prefab_set(e, TextSizeDirty, { 0 });
    // used for font
    zox_prefab_set(e, TextAlignment, { 0 });
    zox_prefab_set(e, TextPadding, { byte2_zero });
    zox_prefab_set(e, TextFontSize, { 0 });
    zox_prefab_set(e, TextResolution, { 0 });
    zox_prefab_set(e, FontOutlineColor, { { 255, 0, 0, 255 }});
    zox_prefab_set(e, FontFillColor, { { 0, 255, 0, 255 }});
    zox_prefab_set(e, FontOutlineThickness, { 2 });
    zox_prefab_set(e, RenderDisabled, { 0 });
    // Do we need to set them? Are font thickness used?
    zox_prefab_set(e, FontThickness, { 1 });
    zox_prefab_set(e, ZigelPrefab, { prefab_glyph });
    return e;
}
