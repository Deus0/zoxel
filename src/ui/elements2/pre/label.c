entity spawn_prefab_label(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("label");
    zox_add_tag(e, Label);
    zox_add_tag(e, TextResizer);
    zox_add_tag(e, FixToLayout);
    return e;
}

/*entity spawn_prefab_label_textured(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("label_background");
    zox_add_tag(e, Label);
    zox_add_tag(e, TextResizer);
    zox_add_tag(e, FixToLayout);
    zox_add_tag(e, FrameTexture);
    zox_prefab_set(e, FrameCorner, { 7 });
    zox_prefab_set(e, OutlineThickness, { 3 });
    zox_prefab_set(e, FillColor, {{ 35, 88, 66, 255 }});
    return e;
}*/
