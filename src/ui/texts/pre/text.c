entity spawn_prefab_text(ecs* world, entity prefab) {

    zox_prefab_child(prefab);
    zox_prefab_name("text");

    // text
    zox_add_tag(e, Zext);
    zox_add_tag(e, Text2D);
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
    zox_prefab_set(e, FontThickness, { 1 });
    zox_prefab_set(e, FontOutlineThickness, { 0 });

    return e;
}
