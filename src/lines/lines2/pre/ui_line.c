entity spawn_prefab_ui_line2D(ecs* world) {
    zox_prefab();
    zox_prefab_name("ui_line2");
    zox_add_tag(e, Line2D);
    zox_add_tag(e, ElementLine2D);
    zox_prefab_set(e, LineData2D, { float4_zero });
    zox_prefab_set(e, LineLocalPosition2, { int4_zero });
    zox_prefab_set(e, LineLocalPositionDirty, { 0 });
    zox_prefab_set(e, LinePosition2, { int4_zero });
    zox_prefab_set(e, LineAnchor, { float4_zero });
    zox_prefab_set(e, LineThickness, { 1 });
    zox_prefab_set(e, CanvasLink, { 0 });
    zox_prefab_set(e, Layer2D, { 0 });    // use to render in order during ui render process
    zox_prefab_set(e, Color, { { 255, 0, 0, 255 } });
    zox_prefab_set(e, ChildIndex, { 0 });
    return e;
}
