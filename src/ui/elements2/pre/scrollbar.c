entity spawn_prefab_scrollbar(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("scrollbar");
    // Select
    zox_add(e, Selectable);
    zox_prefab_set(e, SelectState, { 0 });
    // Click
    zox_add(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });
    zox_add(e, Scrollbar);
    zox_prefab_set(e, ElementMargins, { int2_zero });
    zox_add(e, FixToLayout);
    zox_add(e, FrameTexture);
    zox_prefab_set(e, FrameCorner, { default_button_corner });
    zox_prefab_set(e, OutlineThickness, { default_button_frame_thickness });
    zox_prefab_set(e, FillColor, { button_fill });
    zox_prefab_set(e, OutlineColor, { default_outline_color });
    return e;
}
