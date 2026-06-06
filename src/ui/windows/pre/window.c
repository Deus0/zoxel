void set_prefab_window(ecs* world, entity e) {
    zox_add_tag(e, Window);
    // zox_add_tag(e, WindowRaycastTarget);
    zox_prefab_set(e, HeaderHeight, { 0 });
    // This used for window layering / auto layering
    zox_prefab_set(e, WindowLayer, { 0 });
    zox_prefab_set(e, SetWindowLayer, { 0 });
    // Hierarchy
    // text properties used for spawning list elements
    zox_prefab_set(e, ElementFontSize, { default_font_size });
    // interaction
    zox_add_tag(e, Selectable);
    // zox_prefab_set(e, SelectState, { zox_select_state_none });
    // Layout Constraints
    zox_add_tag(e, BoundToCanvas);
    zox_prefab_set(e, LayoutConstraints, { int4_zero });
}

entity spawn_prefab_window_textured(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("window_textured");
    set_prefab_window(world, e);
    zox_add_tag(e, FixToLayout);
    zox_set(e, FrameCorner, { default_window_corner });
    zox_set(e, Color, { window_fill });
    zox_set(e, OutlineColor, { window_outline });
    return e;
}

entity spawn_prefab_window(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("window");
    set_prefab_window(world, e);
    return e;
}
