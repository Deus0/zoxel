void set_prefab_window(ecs* world, entity e) {
    zox_add(e, Element);
    zox_prefab_set(e, Initialize, { 1 });
    zox_add(e, Window);
    zox_prefab_set(e, WindowLayer, { 0 });
    zox_prefab_set(e, SetWindowLayer, { 0 });
    // Hierarchy
    // text properties used for spawning list elements
    zox_prefab_set(e, ElementFontSize, { default_font_size });
    // interaction
    // zox_add(e, Selectable);
    // Layout Constraints
    zox_add(e, BoundToParent);
    // zox_add(e, BoundToCanvas);
    // zox_prefab_set(e, LayoutConstraints, { int4_zero });
}

entity spawn_prefab_window_textured(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("window_textured");
    set_prefab_window(world, e);
    zox_add(e, FixToLayout);
    zox_set(e, FrameCorner, { default_window_corner });
    zox_set(e, FillColor, { window_fill });
    zox_set(e, OutlineColor, { window_outline });
    return e;
}

entity spawn_prefab_window(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("window");
    set_prefab_window(world, e);
    return e;
}
