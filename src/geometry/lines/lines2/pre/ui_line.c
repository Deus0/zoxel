entity spawn_prefab_ui_line2D(ecs* world) {
    zox_prefab();
    zox_prefab_name("ui_line2");
    zox_add(e, Line2D);
    zox_add(e, ElementLine2D);
    zox_setv(e, LinePoints2, 0);
    zox_setv(e, LocalLinePoints2, 0);
    zox_setv(e, LayoutLinePoints, 0);
    zox_setv(e, LineAnchor, 0);
    zox_setv(e, LineThickness, 1);
    zox_setv(e, Layer, 0);    // use to render in order during ui render process
    zox_set(e, Color, {{ 255, 0, 0, 255 }});
    return e;
}
