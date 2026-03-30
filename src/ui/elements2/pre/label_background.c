entity spawn_prefab_label_background(ecs *world, entity p) {

    zox_prefab_child(p);
    zox_prefab_name("label_background");

    zox_add_tag(e, Label);
    zox_add_tag(e, TextResizer);
    zox_add_tag(e, FrameTexture);
    zox_prefab_set(e, FrameCorner, { 7 });
    zox_prefab_set(e, OutlineThickness, { 3 });
    zox_prefab_set(e, Color, {{ 35, 88, 66, 255 }});

    return e;
}
