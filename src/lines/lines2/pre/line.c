entity spawn_prefab_line2D(ecs *world) {
    zox_prefab();
    zox_prefab_name("line2D");
    zox_add_tag(e, Line2D);
    zox_prefab_set(e, LinePoints2, { 0 });
    zox_prefab_set(e, LineThickness, { 1 });
    zox_prefab_set(e, Color, {{ 0, 255, 255, 255 }});
    zox_prefab_set(e, Layer2D, { 0 });
    return e;
}
